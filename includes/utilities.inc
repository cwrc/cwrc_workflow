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
 *
 * @param unknown $pid
 *   An abstract object pid with workflow.
 */
function islandora_workflow_rest_notify($params, $pid) {
  foreach ($params['accounts'] as $account) {
    $params['mail_account'] = $account;
    drupal_mail('islandora_workflow_rest',
      'notice',
      $account->mail,
      user_preferred_language($account),
      $params
    );
  }
}
