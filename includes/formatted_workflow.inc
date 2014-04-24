<?php
/**
 * @file
 * Supplies formatted workflow.
 */

/**
 * Presents xml using css.
 *
 * @param string $pid
 *   PID of object
 *
 * @return xml
 *   Workflow datastream formatted by css
 */
function islandora_workflow_rest_show_formatted_workflow($pid) {
  $object = islandora_object_load($pid);
  $xml = "";
  if ($object['WORKFLOW']) {
    $xml = $object['WORKFLOW']->content;
  }
  drupal_add_css(drupal_get_path('module', 'islandora_workflow_rest') . '/css/workflow_style.css');
  return "<div class='cwrc_workflow'>$xml</div>";
}
