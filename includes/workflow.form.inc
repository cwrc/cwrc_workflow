<?php
/**
 * @file
 * Workflow entry form.
 */

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
  $all_workflows = drupal_json_decode($json, TRUE);
  $form['islandora_object'] = array('#type' => 'hidden', '#value' => $object->id);

  $form['workflow_wfds_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('View Workflow Datastream'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['workflow_wfds_fs']['datastream_link'] = array(
    '#type' => 'link',
    '#title' => t('View Formatted Datastream'),
    '#href' => "islandora/object/$object->id/datastream/WORKFLOW/view",
  );
  $form['workflow_wfds_fs']['workflow_ds_content'] = array(
    '#type' => 'item',
    '#title' => t('Workflow Datastream Contents'),
    '#markup' => islandora_workflow_rest_pretty_print_ds($object['WORKFLOW']->content),
  );
  $form['workflow_wfs_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('View Workflows'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['workflow_wfs_add_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Workflow'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $options = array();
  // Multi entry.
  if (isset($all_workflows['cwrc']['workflow'][0]['attributes']) && $all_workflows['cwrc']['workflow'] != "") {
    foreach ($all_workflows['cwrc']['workflow'] as $key => $value) {
      $options[$key] = array(
        'workflowID' => $value['attributes']['workflowID'],
        'userID' => $value['attributes']['userID'],
        'category' => (isset($value['activity']['attributes']['category']) ? $value['activity']['attributes']['category'] : ""),
      );
    }
  }
  // Single entry.
  if (isset($all_workflows['cwrc']['workflow']['attributes'])) {
    $options['workflow'] = array(
      'workflowID' => $all_workflows['cwrc']['workflow']['attributes']['workflowID'],
      'userID' => $all_workflows['cwrc']['workflow']['attributes']['userID'],
      'category' => (isset($all_workflows['cwrc']['workflow']['activity']['attributes']['category']) ?
        $all_workflows['cwrc']['workflow']['activity']['attributes']['category'] : ""),
    );
  }

  $header = array(
    'workflowID' => t('Workflow ID'),
    'userID' => t('User ID'),
    'category' => t('Category'),
  );
  $form['workflow_wfs_fs']['table'] = array(
    '#theme' => 'table',
    '#type' => 'tableselect',
    '#header' => $header,
    '#rows' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no workflows."),
  );
  $form = islandora_workflow_rest_build_add_workflow($form);
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
function islandora_workflow_rest_build_add_workflow($form) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/basic_enum');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');

  $assigned_categorys = AssignedCategory::getAllConstants();

  $stamps = array();
  $results = islandora_workflow_rest_get_all_stamps();
  while ($data = $results->fetchObject()) {
    $stamps[$data->stamp] = $data->stamp;
  }

  // Activity input form fieldset.
  $form['workflow_wfs_add_fs']['workflow_fs_activity'] = array(
    '#type' => 'fieldset',
    '#title' => t('Activity'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  // Assigned input form fieldset.
  $form['workflow_wfs_add_fs']['workflow_fs_assigned'] = array(
    '#type' => 'fieldset',
    '#title' => t('Assign'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  // Basic Activity fields.
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_ass_act_category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => $assigned_categorys,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_act_stamp'] = array(
    '#type' => 'select',
    '#title' => t('Stamp'),
    '#options' => $stamps,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['wf_act_status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => array(
      'Complete' => t('Complete'),
      'In Progress' => t('In Progress'),
      'Pending' => t('Pending'),
      'Incomplete' => t('Incomplete'),
    ),
    '#default_value' => 'sample_category',
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
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['act_user'] = array(
    '#type' => 'textfield',
    '#title' => t('User'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['wf_act_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_activity']['workflow_fs_activity_message']['wf_act_message'] = array(
    '#title' => t('Message'),
    '#type' => 'textarea',
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['assign_category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => $assigned_categorys,
    '#default_value' => 'sample_category',
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['assign_process'] = array(
    '#type' => 'textfield',
    '#title' => t('Process'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['assign_note'] = array(
    '#title' => t('Note'),
    '#type' => 'textarea',
  );
  $form['workflow_wfs_add_fs']['submit'][] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  // Assigned input message form fieldset (Notification).
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['user'] = array(
    '#type' => 'textfield',
    '#title' => t('User'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['assign_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => "",
    '#size' => 60,
    '#maxlength' => 128,
  );
  $form['workflow_wfs_add_fs']['workflow_fs_assigned']['workflow_fs_assigned_message']['assign_message'] = array(
    '#title' => t('Message'),
    '#type' => 'textarea',
  );
  return $form;
}

/**
 * Implements hook_form_submit().
 */
function islandora_workflow_rest_entry_form_submit($form, &$form_state) {
  $islandora_object = islandora_object_load($form_state['values']['islandora_object']);

  $assigned_params = array();
  $activity_params = array();

  // Activity parameters.
  $activity_params['category'] = $form_state['values']['wf_ass_act_category'];
  $activity_params['stamp'] = $form_state['values']['wf_act_stamp'];
  $activity_params['status'] = $form_state['values']['wf_act_status'];
  $activity_params['note'] = $form_state['values']['wf_act_note'];
  // Activity Notification.
  $activity_params['recipient'] = $form_state['values']['act_user'];
  $activity_params['subject'] = $form_state['values']['wf_act_subject'];
  $activity_params['body'] = $form_state['values']['wf_act_message'];

  // Assign parameters.
  $assigned_params['category'] = $form_state['values']['assign_category'];
  $assigned_params['process'] = $form_state['values']['assign_process'];
  $assigned_params['note'] = $form_state['values']['assign_note'];
  // Assign Notification.
  $assigned_params['recipient'] = $form_state['values']['user'];
  $assigned_params['subject'] = $form_state['values']['assign_subject'];
  $assigned_params['body'] = $form_state['values']['assign_message'];

  // ToolID was not in the approved mockup. Only in JS API?
  $activity_tool = (isset($form_state['values']['wf_act_tool']) ? $form_state['values']['wf_act_tool'] : "");

  if (!$islandora_object['WORKFLOW']) {
    $workflow = CWRCWorkflowAPI::fromTemplate();
    CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);
  }
  else {
    $workflow = CWRCWorkflowAPI::fromDatastream($islandora_object['WORKFLOW']);
  }

  $activity = new WorkflowItem(WorkflowConst::ACTIVITY,
    $activity_params
  );
  $assigned = new WorkflowItem(WorkflowConst::ASSIGNED,
    $assigned_params
  );
  $entry = new WorkflowItem(WorkflowConst::WORKFLOW,
    array('toolID' => $activity_tool)
  );
  $workflow->addWorkflowStep($islandora_object->id, $entry, $activity, $assigned);
  CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);
}
