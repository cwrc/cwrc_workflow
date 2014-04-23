<?php

/**
 * @file
 * Workflow entry form.
 */

/**
 * Activity/Assigned user auto complete.
 *
 * @param string $string
 *   String to find against matching users.
 */
function islandora_workflow_rest_user_autocomplete($string) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');

  $pieces = explode(",", $string);
  $matches = array();
  $use_realname = module_exists('realname');
  $result = islandora_workflow_rest_find_user_autocomplete($pieces[count($pieces) - 1], $use_realname);
  foreach ($result as $user) {
    $match_name = isset($user->realname) ? $user->realname : $user->name;
    $matches[$match_name] = check_plain($match_name);
  }
  // This may seem crazy, but adds support for csv autocomplete.
  if (count($pieces) > 1) {
    $key_string = "";
    foreach ($pieces as $piece) {
      if ($piece != $pieces[count($pieces) - 1]) {
        $key_string .= $piece . ",";
      }
    }
    $return = array();
    foreach ($matches as $final_key => $final_value) {
      $return[$key_string . $final_value] = $final_value;
    }
    $matches = $return;
  }
  drupal_json_output($matches);
}

/**
 * Workflow Rest entry form.
 *
 * @param unknown $form
 *   Current form.
 * @param unknown $form_state
 *   Current form state.
 * @param AbstractObject $object
 *   The fedora object being processed.
 *
 * @return array
 *   The new form.
 */
function islandora_workflow_rest_entry_form($form, &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/cwrc_workflow');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/basic_enum');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/workflow_item');
  module_load_include('inc', 'php_lib', 'DOMHelpers');

  dom_document_pretty_print_include_files();

  $workflow = NULL;
  if (!$object['WORKFLOW']) {
    $workflow = CWRCWorkflowAPI::fromTemplate($object['WORKFLOW']);
    CWRCWorkflowAPI::updateDatastream($workflow, $object);
  }
  else {
    $workflow = CWRCWorkflowAPI::fromDatastream($object['WORKFLOW']);
  }
  $json = $workflow->workflowToJson();
  $last_workflow_step = $workflow->entryToArray();

  $all_workflows = drupal_json_decode($json, TRUE);
  $form['islandora_object'] = array('#type' => 'hidden', '#value' => $object->id);

  $form['workflow_wfds_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('View Workflow Datastream'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['workflow_wfds_fs']['datastream_link'] = array(
    '#type' => 'link',
    '#title' => t('View Formatted Datastream'),
    '#href' => "islandora_workflow_rest/formatted_workflow/$object->id",
  );
  $form['workflow_wfds_fs']['workflow_ds_content'] = array(
    '#type' => 'item',
    '#title' => t('Workflow Datastream Contents'),
    '#markup' => islandora_workflow_rest_pretty_print_ds($object['WORKFLOW']->content),
  );
  $form['workflow_wfs_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('View Workflow Stamps'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['workflow_wfs_add_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Workflow Stamp'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $options = array();
  // Multi entry.
  if (isset($all_workflows['cwrc']['workflow'][0]['attributes']) && $all_workflows['cwrc']['workflow'] != "") {
    foreach ($all_workflows['cwrc']['workflow'] as $key => $value) {
      $options[$key] = array(
        'date' => $value['attributes']['date'],
        'userID' => $value['attributes']['userID'],
        'stamp' => (isset($value['activity']['attributes']['stamp']) ? $value['activity']['attributes']['stamp'] : ""),
      );
    }
  }
  // Single entry.
  if (isset($all_workflows['cwrc']['workflow']['attributes'])) {
    $options['workflow'] = array(
      'date' => $all_workflows['cwrc']['workflow']['attributes']['date'],
      'userID' => $all_workflows['cwrc']['workflow']['attributes']['userID'],
      'stamp' => (isset($all_workflows['cwrc']['workflow']['activity']['attributes']['stamp']) ?
          $all_workflows['cwrc']['workflow']['activity']['attributes']['stamp'] : ""),
    );
  }

  $header = array(
    'date' => t('Date'),
    'userID' => t('User ID'),
    'stamp' => t('Stamp'),
  );
  $form['workflow_wfs_fs']['table'] = array(
    '#theme' => 'table',
    '#type' => 'tableselect',
    '#header' => $header,
    '#rows' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no workflows."),
  );
  $form = islandora_workflow_rest_build_add_workflow($form, $last_workflow_step);
  return $form;
}

/**
 * Add the workflow entry form to the given form.
 *
 * @param array $form
 *   The drupal form array to append the form to.
 *
 * @return array
 *   The returned form.
 */
function islandora_workflow_rest_build_add_workflow($form, $last_workflow) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/basic_enum');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');

  $collapsed = isset($_GET['lid']) ? FALSE : TRUE;
  $assigned_categories = drupal_map_assoc(array_values(AssignedCategory::getAllConstants()));
  $assigned_categories[NULL] = "Select";
  ksort($assigned_categories);
  $pid_namespace = explode(":", $form['islandora_object']['#value']);

  $stamps = array();
  $stamps[NULL] = "Select";
  $results = islandora_workflow_rest_get_all_stamps(1000);
  while (($data = $results->fetchObject()) == TRUE) {
    $exploded = explode(":", $data->stamp);
    $restriction = variable_get('islandora_workflow_rest_enable_namespace', 0);
    if ($restriction == 0) {
      $stamps[$data->stamp] = $data->stamp . ' - ' . $data->descr;
    }
    else {
      if (strtoupper($pid_namespace[0]) == strtoupper($exploded[0])) {
        $stamps[$data->stamp] = $data->stamp . ' - ' . $data->descr;
      }
    }
  }

  $status_arr = array(
    NULL => t('Select'),
    'c' => t('Complete'),
    'ipr' => t('In Progress'),
    'p' => t('Pending'),
    'i' => t('Incomplete'),
  );
  // Activity input form fieldset.
  $form['workflow_wfs_add_fs']['workflow_fs_activity'] = array(
    '#type' => 'fieldset',
    '#title' => t('Activity'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );
  // Assigned input form fieldset.
  $form['workflow_wfs_add_fs']['workflow_fs_assigned'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assign (optional)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // Basic Activity fields.
  $wf_ass_act_category_default = (isset($last_workflow['response']['workflow']['activity']['attributes']['category']) ?
    $last_workflow['response']['workflow']['activity']['attributes']['category'] : NULL);
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_ass_act_category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#required' => TRUE,
    '#options' => $assigned_categories,
    '#default_value' => $wf_ass_act_category_default,
  );
  $wf_act_stamp_default = (isset($last_workflow['response']['workflow']['activity']['attributes']['stamp']) ?
    $last_workflow['response']['workflow']['activity']['attributes']['stamp'] : NULL);
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_act_stamp'] = array(
    '#type' => 'select',
    '#title' => t('Stamp'),
    '#required' => TRUE,
    '#options' => $stamps,
    '#default_value' => $wf_act_stamp_default,
  );
  $wf_act_status_default = (isset($last_workflow['response']['workflow']['activity']['attributes']['status']) ?
          $last_workflow['response']['workflow']['activity']['attributes']['status'] : NULL);
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_act_status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#required' => TRUE,
    '#options' => $status_arr,
    '#default_value' => $wf_act_status_default,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_act_note'] = array(
    '#title' => t('Note'),
    '#type' => 'textarea',
  );
  // Activity input message form fieldset (Notification).
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['force_act_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force mail notification.'),
    '#description' => t("E-mail notification will be sent to users even if they have opted out of e-mail message delivery."),
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['act_user'] = array(
    '#type' => 'textfield',
    '#id' => 'email-auto-complete-act-user',
    '#title' => t('To'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
    '#autocomplete_path' => 'islandora_workflow_rest/user/autocomplete',
    '#required' => FALSE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['wf_act_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
    '#states' => array(
      'required' => array(
        ':input[name="act_user"]' => array('filled' => TRUE),
      ),
    ),
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['wf_act_message'] = array(
    '#title' => t('Message'),
    '#type' => 'textarea',
    '#states' => array(
      'required' => array(
        ':input[name="act_user"]' => array('filled' => TRUE),
      ),
    ),
  );
  $wf_ass_category_default = (isset($last_workflow['response']['workflow']['assigned']['attributes']['category']) ?
    $last_workflow['response']['workflow']['assigned']['attributes']['category'] : NULL);
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['assign_category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => $assigned_categories,
    '#default_value' => $wf_ass_category_default,
    '#states' => array(
      'required' => array(
        array(':input[name="assign_note"]' => array('filled' => TRUE)),
        array(':input[name="user"]' => array('filled' => TRUE)),
      ),
    ),
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['assign_note'] = array(
    '#title' => t('Note'),
    '#type' => 'textarea',
  );
  $form['workflow_wfs_add_fs']['submit'][] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  // Assigned input message form fieldset (Notification).
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['force_ass_email'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force mail notification.'),
    '#description' => t("E-mail notification will be sent to users even if they have opted out of e-mail message delivery."),
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['user'] = array(
    '#id' => 'email-auto-complete-ass-user',
    '#type' => 'textfield',
    '#title' => t('To'),
    '#default_value' => "",
    '#size' => 60,
    '#autocomplete_path' => 'islandora_workflow_rest/user/autocomplete',
    '#maxlength' => 128,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['assign_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
    '#states' => array(
      'required' => array(
        ':input[name="user"]' => array('filled' => TRUE),
      ),
    ),
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['assign_message'] = array(
    '#title' => t('Message'),
    '#type' => 'textarea',
    '#states' => array(
      'required' => array(
        ':input[name="user"]' => array('filled' => TRUE),
      ),
    ),
  );
  return $form;
}

/**
 * Implements hook_form_validate().
 */
function islandora_workflow_rest_entry_form_validate($form, &$form_state) {
  $use_realname = module_exists('realname');
  // Both notifications require "all or nothing" input.
  if (!empty($form_state['values']['act_user'])) {
    if (empty($form_state['values']['wf_act_subject'])) {
      form_set_error('wf_act_subject', t('Subject field is required when a User is given.'));
    }
    if (empty($form_state['values']['wf_act_message'])) {
      form_set_error('wf_act_message', t('Message field is required when a User is given.'));
    }
    // Load and validate Activity user.
    $activity_users = islandora_workflow_rest_load_users_from_csv($form_state['values']['act_user'], $use_realname);
    if (in_array(FALSE, $activity_users)) {
      form_set_error('act_user', t('The supplied user in Activity notify does not exist.'));
    }
  }
  if (!empty($form_state['values']['user'])) {
    if (empty($form_state['values']['assign_subject'])) {
      form_set_error('assign_subject', t('Subject field is required when a User is given.'));
    }
    if (empty($form_state['values']['assign_message'])) {
      form_set_error('assign_message', t('Message field is required when a User is given.'));
    }
    if (empty($form_state['values']['assign_category'])) {
      form_set_error('assign_category', t('Assign Category is required when a Notification User is given.'));
    }
    // Load and validate Assign user.
    $assign_users = islandora_workflow_rest_load_users_from_csv($form_state['values']['user'], $use_realname);
    if (in_array(FALSE, $assign_users)) {
      form_set_error('user', t('The supplied user in Assign notify does not exist.'));
    }
  }
  if (empty($form_state['values']['assign_category']) && $form_state['values']['assign_note'] != "") {
    form_set_error('assign_category', t('Assign Category is required when a Assign Note is specified.'));
  }
}

/**
 * Implements hook_form_submit().
 */
function islandora_workflow_rest_entry_form_submit($form, &$form_state) {
  $use_realname = module_exists('realname');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');

  $islandora_object = islandora_object_load($form_state['values']['islandora_object']);
  $assigned_params = array();
  $activity_params = array();

  // Activity parameters.
  $activity_params['category'] = $form_state['values']['wf_ass_act_category'];
  $activity_params['stamp'] = $form_state['values']['wf_act_stamp'];
  $activity_params['status'] = $form_state['values']['wf_act_status'];
  $activity_params['note'] = $form_state['values']['wf_act_note'];
  // Activity Notification.
  $activity_recipients = islandora_workflow_rest_transform_user_csv($form_state['values']['act_user']);
  $activity_params['recipient'] = $activity_recipients;
  $activity_params['subject'] = $form_state['values']['wf_act_subject'];
  $activity_params['body'] = $form_state['values']['wf_act_message'];

  // Assign is Optional.
  $assigned = NULL;
  if (!empty($form_state['values']['assign_category'])) {
    // Assign parameters.
    $assigned_params['category'] = $form_state['values']['assign_category'];
    $assigned_params['note'] = $form_state['values']['assign_note'];
    // Assign Notification.
    $assigned_recipients = islandora_workflow_rest_transform_user_csv($form_state['values']['user']);
    $assigned_params['recipient'] = $assigned_recipients;
    $assigned_params['subject'] = $form_state['values']['assign_subject'];
    $assigned_params['body'] = $form_state['values']['assign_message'];
    $assigned = new WorkflowItem(WorkflowConst::ASSIGNED, $assigned_params);
  }

  // ToolID was not in the approved mock-up. Only in JS API?
  $activity_tool = (isset($form_state['values']['wf_act_tool']) ? $form_state['values']['wf_act_tool'] : "");

  if (!$islandora_object['WORKFLOW']) {
    $workflow = CWRCWorkflowAPI::fromTemplate();
    CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);
  }
  else {
    $workflow = CWRCWorkflowAPI::fromDatastream($islandora_object['WORKFLOW']);
  }

  $activity = new WorkflowItem(WorkflowConst::ACTIVITY, $activity_params);
  $entry = new WorkflowItem(WorkflowConst::WORKFLOW, array('toolID' => $activity_tool));

  // Get the last workflow step, if exists.
  $last_workflow = $workflow->entryToArray();
  if ($last_workflow) {
    $last_wf_string = islandora_workflow_rest_prepare_workflow_string($last_workflow, "previous");
  }
  else {
    $last_wf_string = "";
  }
  $workflow->addWorkflowStep($islandora_object->id, $entry, $activity, $assigned);

  CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);

  // At this point, the new workflow has been added
  // to the WORKFLOW Datastream.
  drupal_set_message(t("Workflow entry successfully submitted."), "status");

  // Get the current workflow step.
  $this_workflow = $workflow->entryToArray();
  $this_wf_string = islandora_workflow_rest_prepare_workflow_string($this_workflow, "current");
  if ($form_state['values']['act_user'] != "") {
    // Email activity notification.
    $params['body'] = $form_state['values']['wf_act_message'] . "\n\n" .
        $this_wf_string . "\n\n" . $last_wf_string;
    $params['link'] = url("islandora/object/$islandora_object->id/workflow");
    $params['subject'] = $form_state['values']['wf_act_subject'];
    $params['accounts'] = islandora_workflow_rest_load_users_from_csv($form_state['values']['act_user'], $use_realname);
    $params['force_email'] = $form_state['values']['force_act_email'];
    islandora_workflow_rest_notify($params);
  }
  if (!empty($form_state['values']['user']) && isset($assigned)) {
    // Email Assigned notification.
    $params['body'] = $form_state['values']['assign_message'] . "\n\n" .
        $this_wf_string . "\n\n" . $last_wf_string;
    $params['link'] = url("islandora/object/$islandora_object->id/workflow");
    $params['subject'] = $form_state['values']['assign_subject'];
    $params['accounts'] = islandora_workflow_rest_load_users_from_csv($form_state['values']['user'], $use_realname);
    $params['force_email'] = $form_state['values']['force_ass_email'];
    islandora_workflow_rest_notify($params);
  }
  if (isset($_GET['lid'])) {
    $lid = $_GET['lid'];
    db_update('islandora_workflow_rest_ds_logs')
        ->fields(array('state' => 'tagged'))
        ->condition('lid', $lid)
        ->execute();
    drupal_goto('cwrc_workflow');
  }
}
