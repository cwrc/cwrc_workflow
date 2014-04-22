<?php

/**
 * @file
 * An primary Solr display for running XQuery queries.
 * @todo
 *   Remind the user that Solr may not be up to date.
 */

/**
 * Extention of IslandoraSolrResults to create an alternative display type.
 */
class IslandoraSolrResultsWorkflowRest extends IslandoraSolrResults {

  /**
   * Constructor.
   */
  public function __construct() {
    parent::__construct();
  }

  /**
   * Renders the Solr results as a table.
   *
   * @see displayResults()
   *
   * @param array $results
   *   The Solr results from IslandoraSolrQueryProcessor::solrResult
   *
   * @return array
   *   Rendered Solr results
   */
  public function printResults($results) {
    $report = $_GET['workflow_report'];
    if (isset($_GET['workflow_report'])) {
      $report = $_GET['workflow_report'];
      $records = islandora_workflow_rest_get_all_records();
      $chosen = $records[$report];
      $fields = unserialize($chosen['solr_terms']);
      foreach ($fields as $field) {
        $label = $field['display_label'];
        $value = $field['hyperlink'] ? "islandora/object/$label" : $label;
        $solr_fields[$field['solr_field']] = array(
          'label' => $label,
          'class' => str_replace('_', '-', $field['solr_field']),
          'value' => $field['solr_field'],
        );
      }
    }
    $solr_results = islandora_solr_prepare_solr_results($results);
    $objects = $solr_results['response']['objects'];
    $object_results = islandora_solr_prepare_solr_doc($objects);
    foreach ($object_results as &$result) {
      $solr_doc = $result['solr_doc'];
      foreach ($solr_doc as $term => $valueset) {
        if (array_key_exists($term, $solr_fields)) {
          $new_doc[$term] = $valueset;
        }
      }
      $result['solr_doc'] = $new_doc;
    }
    $elements = array();
    $elements['solr_total'] = $solr_results['response']['numFound'];
    $elements['solr_start'] = $solr_results['response']['start'];

    // Return themed search results.
    return theme('islandora_solr', array('results' => $object_results, 'elements' => $elements));
  }

}
