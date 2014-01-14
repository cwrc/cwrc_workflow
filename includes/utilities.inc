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
  $query = db_select('islandora_workflow_rest_stamps', 'r')->extend('PagerDefault');
  $query = $query->fields('r', array("lid", "stamp", "descr"));
  $query = $query->extend('TableSort')->limit($limit);
  $stamps = $query->execute();

  return $stamps;
}
