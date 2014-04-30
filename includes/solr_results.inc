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
          'linked' => $field['hyperlink'],
        );
      }
    }
    drupal_set_title($chosen['name']);
    // Pull unfiltered results.
    $query = str_replace('islandora/search/', '', $_GET['q']);
    $url = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));
    $solr = new Apache_Solr_Service($url['host'], $url['port'], $url['path'] . '/');
    $solr->setCreateDocuments(FALSE);
    try {
      $local_results = $solr->search($query, 0, 1000, array());
      $json = json_decode($local_results->getRawResponse(), TRUE);
      foreach ($json['response']['docs'] as $candidate) {
        $full_results[$candidate['PID']] = $candidate;
      }
    }
    catch (Exception $e) {
      watchdog_exception('Islandora Rest Workflow', $e, 'Got an exception while searching.', array(), WATCHDOG_ERROR);
    }

    $solr_results = islandora_solr_prepare_solr_results($results);
    $objects = $solr_results['response']['objects'];
    $object_results = islandora_solr_prepare_solr_doc($objects);
    foreach ($object_results as &$result) {
      $pid = $result['PID'];
      $full_result = $full_results[$pid];
      $new_solr_doc = array();
      foreach ($fields as $field) {
        $term = $field['solr_field'];
        // Mutli-valued fields are returned as arrays.
        if (array_key_exists($term, $full_result)) {
          $values = $full_result[$term];
          if (is_array($values)) {
            $value = $values[0];
          }
          else {
            $value = $values;
          }
          $new_solr_doc[$term] = array(
            'class' => str_replace('_', '-', $term),
            'value' => $solr_fields[$term]['linked'] ? l($value, "islandora/object/$pid") : $value,
            'label' => $solr_fields[$term]['label'],
          );
        }
      }
      $result['solr_doc'] = $new_solr_doc;
    }
    $elements = array();
    $elements['solr_total'] = $solr_results['response']['numFound'];
    $elements['solr_start'] = $solr_results['response']['start'];
    // Return themed search results.
    return l(t("Return to reports"), "islandora_workflow_rest/reports") . theme('islandora_solr', array('results' => $object_results, 'elements' => $elements));
  }

}
