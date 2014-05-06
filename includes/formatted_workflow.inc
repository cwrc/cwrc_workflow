<?php
/**
 * @file
 * Supplies formatted workflow.
 */

/**
 * Presents xml using css.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing an object within Fedora.
 *
 * @return string
 *   Workflow datastream formatted by css
 */
function islandora_workflow_rest_show_formatted_workflow(AbstractObject $object) {
  if ($object && isset($object['WORKFLOW'])) {
    $xml = $object['WORKFLOW']->content;
    $css_path = file_create_url(drupal_get_path('module', 'islandora_workflow_rest') . '/css/workflow_style.css');
    // Need to replace the XML version information out as the processing
    // instruction must come first.
    $xml = str_replace('<?xml version="1.0" encoding="UTF-8"?>', '', $xml);
    header('Content-Type: text/xml');
    $writer = new XMLWriter();
    $writer->openURI('php://output');
    $writer->startDocument('1.0', 'utf-8');
    $writer->writePI('xml-stylesheet', 'href="' . file_create_url($css_path) . '"');
    $writer->writeRaw($xml);
    $writer->endDocument();
    $writer->flush();
  }
}
