<?php

/**
 * @file
 * Defines workflow management functions.
 */

/**
 * Displays workflow management table.
 *
 * @global type $user
 *
 * @return array
 *   Drupal Form
 */
function cwrc_workflow_management_form($form, &$form_state) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/cwrc_workflow');
  module_load_include('inc', 'islandora', 'includes/datastream.version');
  global $user;
  $user_id = $user->uid;
  $query = "select * from {islandora_workflow_rest_ds_logs}
             where usr = '$user_id'
             and state = 'new'
      order by pid, dsid desc";
  $results = db_query($query);
  $lines = array();

  while ($result = $results->fetchAssoc()) {
    $object = islandora_object_load($result['pid']);
    if (!$object) {
      db_update('islandora_workflow_rest_ds_logs')
          ->fields(array('state' => 'object_deleted'))
          ->condition('lid', $result['lid'])
          ->execute();
      continue;
    }
    $date_to_find = intval($result['timestamp']);
    $version_number = NULL;
    $revision_link = "";
    $datastream = $object[$result['dsid']];
    if ($datastream) {
      foreach ($datastream as $version_number => $version) {
        $date_from_trail = strtotime((string) $version->createdDate);
        if ($date_to_find == $date_from_trail) {
          $revision_text = $version->createdDate;
          $revision_link = l($revision_text, 'islandora/object/' . $result['pid'] . '/datastream/' . $result['dsid'] . "/version/$version_number/view");
        }
      }
    }
    $last_workflow_step = NULL;
    if ($object['WORKFLOW']) {
      $workflow = CWRCWorkflowAPI::fromDatastream($object['WORKFLOW']);
      $last_workflow_step = $workflow->entryToArray();
    }
    $stamp_exists = isset($last_workflow_step['response']['workflow']['activity']['attributes']['stamp']);
    $last_stamp = $stamp_exists ? $last_workflow_step['response']['workflow']['activity']['attributes']['stamp'] : t('None');
    $key = $result['lid'];
    $link = l($object->label, 'islandora/object/' . $object->id . '/workflow', $options = array('query' => array('lid' => $key)));
    $lines[$key]['object'] = $link;
    $lines[$key]['dsid'] = $result['dsid'];
    $lines[$key]['timestamp'] = gmdate("F j, Y, g:i a", $result['timestamp']);
    $lines[$key]['last_stamp'] = $last_stamp;
    $lines[$key]['revision'] = $revision_link;
  }

  $header = array(
    'object' => t('Tag Workflow'),
    'dsid' => t('DSID'),
    'timestamp' => t('Modified'),
    'last_stamp' => t('Last Stamp'),
    'revision' => t('Go to Revision'),
  );

  $form['logged_events'] = array(
    '#type' => 'tableselect',
    '#title' => t('Logged Events'),
    '#header' => $header,
    '#options' => $lines,
    '#attributes' => '',
    '#empty' => t("There are no logged events to be tagged."),
  );
  if (count($lines) > 0) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Remove checked log entries.'),
    );
  }
  return $form;
}

/**
 * Submit form function for management form.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal form state
 */
function cwrc_workflow_management_form_submit($form, &$form_state) {
  $logged_events = array_filter($form_state['values']['logged_events']);
  $serialized = serialize($logged_events);
  drupal_goto("islandora_workflow_rest/skip_entries/$serialized");
}

/**
 * The workflow skip entries confirmation form.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $lids
 *   Serialized array of lids to be deleted.
 *
 * @return array
 *   The drupal form definition.
 */
function cwrc_workflow_skip_entries_form(array $form, array &$form_state, $lids) {
  $lids = unserialize($lids);
  $form_state['lids'] = $lids;
  return confirm_form($form, t('Are you sure you want to skip %count log entries?', array('%count' => count($lids))), "cwrc_workflow", t('This action cannot be undone.'), t('Skip checked entries'), t('Cancel')
  );
}

/**
 * Marks checked log entrieds as skipped.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function cwrc_workflow_skip_entries_form_submit(array $form, array &$form_state) {
  foreach ($form_state['lids'] as $lid) {
    db_update('islandora_workflow_rest_ds_logs')
        ->fields(array('state' => 'skipped'))
        ->condition('lid', $lid)
        ->execute();
  }
  drupal_set_message(t("Checked entries skipped"));
  drupal_goto('cwrc_workflow');
}
