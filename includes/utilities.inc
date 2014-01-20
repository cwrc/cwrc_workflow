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
 * Retrieve all stamp object.
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
  $query = $query->extend('PagerDefault')->limit($limit);
  $stamps = $query->execute();

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