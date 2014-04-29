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
function islandora_workflow_rest_cwrc_workflow_management_form($form, &$form_state) {
  module_load_include('inc', 'islandora', 'includes/datastream.version');
  global $user;
  $user_id = $user->uid;
  $sort = 'ASC';
  $order = 'timestamp';
  if (isset($_GET['sort']) && isset($_GET['order'])) {
    $sort = strtoupper($_GET['sort']);
    switch ($_GET['order']) {
      case t('Tag Workflow'):
        $order = 'object_label';
        break;

      case t('DSID'):
        $order = 'dsid';
        break;

      case t('Modified'):
        $order = 'timestamp';
        break;

      case t('Last Stamp'):
        $order = 'timestamp';
        break;

      default:
        $order = 'timestamp';
    }
  }

  $query = db_select('islandora_workflow_rest_ds_logs', 'i')
      ->fields('i')
      ->condition('usr', $user_id)
      ->condition('state', 'new')
      ->orderBy($order, $sort);
  $results = $query->execute();
  $lines = array();
  while ($result = $results->fetch()) {
    $object = islandora_object_load($result->pid);
    if (!$object) {
      db_update('islandora_workflow_rest_ds_logs')
          ->fields(array('state' => 'object_deleted'))
          ->condition('lid', $result->lid)
          ->execute();
      continue;
    }
    $date_to_find = intval($result->timestamp);
    $version_number = NULL;
    $revision_link = "";
    $datastream = $object[$result->dsid];
    if ($datastream) {
      foreach ($datastream as $version_number => $version) {
        $date_from_trail = strtotime((string) $version->createdDate);
        if ($date_to_find == $date_from_trail) {
          $revision_text = $version->createdDate;
          $revision_link = l($revision_text, 'islandora/object/' . $result->pid . '/datastream/' . $result->dsid . "/version/$version_number/view");
        }
      }
    }
    $last_stamp = $result->last_stamp;
    $key = $result->lid;
    $link = l($result->object_label, 'islandora/object/' . $object->id . '/workflow', $options = array('query' => array('lid' => $key)));
    $lines[$key]['object'] = $link;
    $lines[$key]['dsid'] = $result->dsid;
    $lines[$key]['timestamp'] = islandora_workflow_rest_convert_timestamp_to_user_local($result->timestamp);
    $lines[$key]['last_stamp'] = $last_stamp;
    $lines[$key]['revision'] = $revision_link;
  }

  $header = array(
    'object' => array(
      'data' => t('Tag Workflow'),
      'field' => 'pid',
    ),
    'dsid' => array(
      'data' => t('DSID'),
      'field' => 'dsid',
    ),
    'timestamp' => array(
      'data' => t('Modified'),
      'field' => 'timestamp',
      'sort' => 'desc',
    ),
    'last_stamp' => array(
      'data' => t('Last Stamp'),
      'field' => 'timestamp',
    ),
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
 * Convert stored timestamp to the users timezone.
 *
 * @param int $timestamp
 *   The timestamp to convert.
 * @param string $format
 *   The format to use when converting.
 *
 * @return string
 *   The newly formatted date time string.
 */
function islandora_workflow_rest_convert_timestamp_to_user_local($timestamp, $format = "F j, Y, g:i a") {
  // Use the Unix Timestamp format to construct a new DateTime object.
  // Using '@$var' constructs DateTime with no timezone.
  $date = new DateTime("@$timestamp");
  // Set the timezone based on user selected, or site default by default.
  date_timezone_set($date, timezone_open(drupal_get_user_timezone()));
  return date_format($date, $format);
}

/**
 * Submit form function for management form.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal form state
 */
function islandora_workflow_rest_cwrc_workflow_management_form_submit($form, &$form_state) {
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
function islandora_workflow_rest_cwrc_workflow_skip_entries_form(array $form, array &$form_state, $lids) {
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
function islandora_workflow_rest_cwrc_workflow_skip_entries_form_submit(array $form, array &$form_state) {
  foreach ($form_state['lids'] as $lid) {
    db_update('islandora_workflow_rest_ds_logs')
        ->fields(array('state' => 'skipped'))
        ->condition('lid', $lid)
        ->execute();
  }
  drupal_set_message(t("Checked entries skipped"));
  drupal_goto('cwrc_workflow');
}
