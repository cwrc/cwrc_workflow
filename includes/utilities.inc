<?php
/**
 * @file
 * Holds helper functions for islandora_workflow_rest.
 */

/**
 * Set a new stamp.
 *
 * @param string $stamp
 *   The stamp to persist.
 */
function islandora_workflow_rest_set_stamp($stamp, $descr = "") {
  db_insert('islandora_workflow_rest_stamps')
  ->fields(array(
    'stamp' => $stamp,
    'descr' => $descr,
  ))
  ->execute();
}

/**
 * Remove an object's embargo information.
 *
 * @param string $stamp
 *   The stamp to remove.
 */
function islandora_workflow_rest_remove_stamp($stamp) {
  db_delete('islandora_workflow_rest_stamps')
  ->condition('stamp', $stamp)
  ->execute();
}

/**
 * Get a stamp by stamp name.
 *
 * @param string $stamp
 *   The stamp to check for/get.
 *
 * @return string
 *   The human readable stamp name.
 */
function islandora_workflow_rest_get_stamp($stamp) {
  $list = db_select('islandora_workflow_rest_stamps', 'c')
  ->fields('c')
  ->condition('stamp', $stamp)
  ->execute();
  $data = $list->fetchObject();
  $return_info = (isset($data->stamp) ? $data->stamp : "");
  return $return_info;
}

/**
 * Retrieve all stamp objects.
 *
 * @param int $limit
 *   The limit to set on the query.
 *
 * @return array
 *   The list of workflow stamps.
 */
function islandora_workflow_rest_get_all_stamps($limit = 10) {
  $query = db_select('islandora_workflow_rest_stamps', 'r');
  $query->fields('r', array("lid", "stamp", "descr"));
  $table_sort = $query->extend('TableSort');
  $pager = $table_sort->extend('PagerDefault')->limit($limit);
  $stamps = $pager->execute();
  return $stamps;
}

/**
 * Set a new Conten Model log entry.
 * 
 * @param string $type
 *   The type of edit (EX: add, delete...).
 * @param string $user
 *   The user making the modification.
 * @param string $dsid
 *   The DataStream ID.
 * @param string $delay
 *   The Delay, in seconds, between logging.
 * @param string $pid
 *   The Content Model's PID.
 */
function islandora_workflow_rest_set_ds_log($type, $user, $dsid, $delay, $pid) {
  db_insert('islandora_workflow_rest_ds_logs')
  ->fields(
    array(
      'type' => $type,
      'usr' => $user,
      'dsid' => $dsid,
      'delay' => $delay,
      'pid' => $pid,
    )
  )
  ->execute();
}

/**
 * Remove a Datastream Log Entry.
 *
 * @param string $pid
 *   The Content Model's PID.
 * @param string $dsid
 *   The DataStream ID.
 * @param string $user
 *   The user making the modification.
 */
function islandora_workflow_rest_remove_ds_log($pid, $dsid = "", $user = "") {
  db_delete('islandora_workflow_rest_ds_logs')
  ->condition('pid', $pid)
  ->condition('dsid', $dsid)
  ->condition('usr', $user)
  ->execute();
}

/**
 * Return a list of Data Stream logs by PID.
 *
 * @param string $pid
 *   The pid to retrieve DS Logs.
 *
 * @return array
 *   An array of Data Stream logs by PID.
 */
function islandora_workflow_rest_get_ds_logs_by_pid($pid) {
  $list = db_select('islandora_workflow_rest_ds_logs', 'c')
  ->fields('c')
  ->condition('pid', $pid)
  ->execute();
  return $list;
}

/**
 * Retrieve existing ds logs, via pager.
 *
 * @param int $limit
 *   The number to limit the page results to.
 *
 * @return array
 *   An array of Monitor log entry's.
 */
function islandora_workflow_rest_get_all_ds_logs($limit = 10) {
  $query = db_select('islandora_workflow_rest_ds_logs', 'r');
  $query->fields('r', array("lid", "type", "dsid", "delay", "pid", "usr"));
  $table_sort = $query->extend('TableSort');
  $pager = $table_sort->extend('PagerDefault')->limit($limit);
  $ds_logs = $pager->execute();

  return $ds_logs;
}

/**
 * Set a new ds log admin entry.
 *
 * @param string $dsid
 *   The DSID to update to.
 * @param string $delay
 *   The new Delay, in seconds, between logging.
 * @param string $pid
 *   The PID of the Content Model to update.
 * @param string $label
 *   The label of the Content Model.
 * @param int $monitor
 *   0 or 1, indicating that this CM is being monitored.
 */
function islandora_workflow_rest_set_ds_log_admin($dsid, $delay, $pid, $label, $monitor = 0) {
  db_insert('islandora_workflow_rest_ds_log_admin')
  ->fields(
    array(
      'dsid' => $dsid,
      'delay' => $delay,
      'pid' => $pid,
      'label' => $label,
      'monitor' => $monitor,
    )
  )
  ->execute();
}

/**
 * Update an existing ds log admin entry.
 *
 * @param string $pid
 *   The PID of the Content Model to update.
 * @param string $dsid
 *   The DSID to update to.
 * @param string $delay
 *   The new Delay, in seconds, between logging.
 * @param int $monitor
 *   0 or 1, indicating that this CM is being monitored.
 */
function islandora_workflow_rest_update_ds_log_admin($pid, $dsid = "", $delay = "", $monitor = 0) {
  db_update('islandora_workflow_rest_ds_log_admin')
  ->fields(array('dsid' => $dsid, 'delay' => $delay, 'monitor' => $monitor))
  ->condition('pid', $pid)
  ->execute();
}

/**
 * Get ds admin log entry by pid.
 *
 * @param string $pid
 *   The PID to retrieve ds log entrys for.
 *
 * @return array
 *   An array of ds log entry's.
 */
function islandora_workflow_rest_get_ds_log_admin_by_pid($pid) {
  $list = db_select('islandora_workflow_rest_ds_log_admin', 'c')
  ->fields('c')
  ->condition('pid', $pid)
  ->execute();
  return $list;
}

/**
 * Delete ds admin log entry.
 *
 * @param string $pid
 *   The Content Model PID to remove entry.
 */
function islandora_workflow_rest_delete_ds_logs_admin($pid) {
  db_delete('islandora_workflow_rest_ds_log_admin')
  ->condition('pid', $pid)
  ->execute();
}

/**
 * Get all ds admin logs.
 *
 * @param int $limit
 *   Limit the results, used via pager.
 *
 * @return array
 *   An array of admin logs.
 */
function islandora_workflow_rest_get_all_ds_logs_admin($limit = 10) {
  $query = db_select('islandora_workflow_rest_ds_log_admin', 'r');
  $query->fields('r', array("lid", "pid", "label", "dsid", "delay", "monitor"));
  $table_sort = $query->extend('TableSort');
  $pager = $table_sort->extend('PagerDefault')->limit($limit);
  $ds_logs = $pager->execute();
  return $ds_logs;
}

/**
 * Gets the payload from a PUT/DELETE request.
 *
 * HTTP PUT/DELETE isn't well supported in PHP, so this is our only option.
 *
 * @return array
 *   The JSON decoded payload if defined.
 */
function islandora_workflow_rest_get_request_body() {
  $body = &drupal_static(__FUNCTION__);
  if (isset($body)) {
    return $body;
  }
  if (isset($_SERVER['CONTENT_LENGTH']) && $_SERVER['CONTENT_LENGTH'] > 0) {
    $stdin = fopen("php://input", "r");
    $body = fread($stdin, $_SERVER['CONTENT_LENGTH']);
    $content_type = isset($_SERVER["CONTENT_TYPE"]) ? $_SERVER["CONTENT_TYPE"] : '';
    switch ($content_type) {
      case 'application/json':
      default:
        // At the moment we are assume only JSON will be included in the request
        // body. To support multi-part PUT request we would have to implement
        // some request body parser here.
        $body = drupal_json_decode($body);
        break;
    }
  }
  return $body;
}

/**
 * Get the request parameters.
 *
 * @return array
 *   An array containing the request parameters.
 */
function islandora_workflow_rest_get_request_parameters() {
  switch ($_SERVER['REQUEST_METHOD']) {
    case 'GET':
      return $_GET;

    case 'POST':
      // Can POST Fields or Content Body, prefer Fields.
      if (empty($_POST)) {
        return islandora_workflow_rest_get_request_body();
      }
      return $_POST;

    case 'PUT':
    case 'DELETE':
      return islandora_workflow_rest_get_request_body();

    default:
      throw new Exception('Method Not Allowed', 405);
  }
}

/**
 * Admin autocomplete callback which returns solr fields from Luke.
 *
 * @param string $string
 *   String filled out in the autocomplete textfield.
 *
 * @return json
 *   A json array containing the Solr luke values that contain the given string.
 */
function islandora_workflow_rest_autocomplete_luke($string = '') {
  module_load_include('inc', 'islandora_solr', 'includes/luke');
  $luke = islandora_solr_get_luke();
  $result = array();
  foreach ($luke['fields'] as $term => $value) {
    if (stripos($term, $string) !== FALSE) {
      // Search case insensitive, but keep the case on replace.
      $term_str = preg_replace("/$string/i", "<strong>\$0</strong>", $term);

      // Add strong elements to highlight the found string.
      $result[$term] = $term_str . '<strong style="position: absolute; right: 5px;">(' . $value['type'] . ')</strong>';
    }
  }
  // Sort alphabetically.
  ksort($result);

  drupal_json_output($result);
  exit();
}

/**
 * Pretty print datastream content.
 *
 * @param string $content
 *   An objects datastream content.
 *
 * @return array
 *   Formated xml/html for content.
 */
function islandora_workflow_rest_pretty_print_ds($content) {
  module_load_include('inc', 'php_lib', 'DOMHelpers');
  $doc = new DOMDocument();
  $doc->loadXML($content);
  $return = dom_document_pretty_print($doc, TRUE);
  return $return;
}

/**
 * Retrive an array of accounts for a give CSV of usernames.
 *
 * @param string $string
 *   CSV string with valid drupal user names.
 *
 * @return array
 *   An array of user accounts.
 */
function islandora_workflow_rest_load_users_from_csv($string) {
  $accounts = array();
  $pieces = explode(",", $string);
  foreach ($pieces as $key => $value) {
    $accounts[] = user_load_by_name($value);
  }
  return $accounts;
}

/**
 * Transform a workflow entry into email text.
 *
 * @param array $workflow_item
 *   The workflow item as array to transform.
 *
 * @return string
 *   The string entry for notification emails.
 */
function islandora_workflow_rest_prepare_workflow_string($workflow_item) {
  // Check if there IS a workflow entry.
  $text = "";
  if (isset($workflow_item['response'])) {
    $text = "Workflow Entry (" . $workflow_item['response']['workflow']['attributes']['workflowID'] . "):\n";
    $text .= "--Stamp: " . $workflow_item['response']['workflow']['activity']['attributes']['category'] . "\n";
    $text .= "--Category: " . $workflow_item['response']['workflow']['activity']['attributes']['stamp'] . "\n";
    $text .= "--Status: " . $workflow_item['response']['workflow']['activity']['attributes']['status'] . "\n";
  }
  return $text;
}

/**
 * Email helper, using drupal_mail.
 *
 * @param array $params
 *   Required email notification parameters.
 */
function islandora_workflow_rest_notify($params) {
  foreach ($params['accounts'] as $account) {
    $params['mail_account'] = $account;
    drupal_mail(
      'islandora_workflow_rest',
      'notice',
      $account->mail,
      user_preferred_language($account),
      $params,
      "donotreplay@site.com",
      TRUE
    );
  }
}

/**
 * Create the configure content models table.
 *
 * @return array
 *   The form as array to be rendered by drupal.
 */
function islandora_workflow_rest_admin_cm_table_form() {
  global $user;

  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
  $form = array();

  $data = islandora_workflow_rest_get_all_ds_logs_admin(10);
  $options = array();
  $item_checked = array();
  while ($content_model = $data->fetchObject()) {
    $monitor_datastream = array(
      '#type' => 'textfield',
      '#value' => $content_model->dsid,
      '#title' => 'Datastream ID',
      '#title_display' => 'invisible',
      '#name' => $content_model->pid . '[dsid_entry]',
    );
    $logging_delay = array(
      '#type' => 'textfield',
      '#value' => $content_model->delay,
      '#title' => 'Logging Delay',
      '#title_display' => 'invisible',
      '#name' => $content_model->pid . '[delay_entry]',
    );
    $options[$content_model->pid]['pid'] = $content_model->pid;
    $options[$content_model->pid]['label'] = $content_model->label;
    $options[$content_model->pid]['dsid'] = array('data' => $monitor_datastream);
    $options[$content_model->pid]['delay'] = array('data' => $logging_delay);
    $item_checked[$content_model->pid] = $content_model->monitor;
  }
  $header = array(
    'pid' => t('PID'),
    'label' => t('Label'),
    'dsid' => t('Datastream ID'),
    'delay' => t('Logging Delay'),
  );
  $form['content_model_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $item_checked,
    '#attributes' => '',
    '#empty' => t("There are no content models??"),
  );
  return $form;
}
