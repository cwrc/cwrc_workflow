<?php

/**
 * @file
 * File database utilities for reporting.
 */

/**
 * Creates report record.
 *
 * @param array $record
 *   Associative array of $field => $value pairs.
 */
function islandora_workflow_rest_add_record(array $record) {
  db_insert('islandora_workflow_rest_reports')
      ->fields($record)
      ->execute();
}
/**
 * Updates report record.
 *
 * @param array $record
 *   Associative array of $field => $value pairs.
 * @param int $lid
 *   Identifier of record being updated
 */
function islandora_workflow_rest_update_record(array $record, $lid) {
  db_update('islandora_workflow_rest_reports')
      ->fields($record)
      ->condition('lid', $lid)
      ->execute();
}

/**
 * Retrieves Record by LID.
 *
 * @param string $lid
 *   Record's ID
 *
 * @return array
 *   Record as array
 */
/**
 * Retrieves record.
 *
 * @param string $field
 *   Field to be searched
 * @param string $value
 *   Value to match
 *
 * @return array
 *   Record as $field => $value pairs.
 */
function islandora_workflow_rest_get_record($field, $value) {
  $record = db_select('islandora_workflow_rest_reports', 'c')
          ->fields('c')
          ->condition($field, $value)
          ->execute()->fetchAssoc();

  return $record;
}

/**
 * Gets all records from table.
 *
 * @return array
 *   Associative array of table fields => values
 */
function islandora_workflow_rest_get_all_records() {
  $records = array();
  $results = db_select('islandora_workflow_rest_reports', 'c')
          ->fields('c')
          ->orderBy('project', 'DESC')
          ->execute();
  while ($record = $results->fetchAssoc()) {
    $records[$record['lid']] = $record;
  }
  return $records;
}
