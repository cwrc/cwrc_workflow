<?php
/**
 * @file
 * Admin form implementations.
 */

/**
 * Admin page for adding additional stamps.
 *
 * @param array $form
 *   The current form.
 * @param array $form_state
 *   The current form_state.
 *
 * @return array
 *   The form array.
 */
function islandora_workflow_rest_admin($form, &$form_state) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');
  $form['workflow_stamp_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Stamp'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['workflow_query_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure SOLR Query'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['workflow_query_fs']['workflow_solr_field'] = array(
    '#type' => 'textfield',
    '#title' => t('SOLR Query Field'),
    '#description' => t('Configure the SOLR query Workflow will use with the rest interface.'),
    '#size' => 45,
    '#autocomplete_path' => 'islandora_workflow_rest/autocomplete_luke',
    '#default_value' => variable_get('workflow_solr_field', ISLANDORA_WORKFLOW_REST_SOLR_DEFAULT_FIELD),
  );
  $form['workflow_stamp_fs']['workflow_stamp_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Add additional stamps'),
    '#description' => t('Include additional stamp values (used when adding an activity via workflow API).'),
    '#default_value' => "",
    '#weight' => 3,
  );
  $form['workflow_stamp_fs']['workflow_stamp_desc'] = array(
    '#type' => 'textfield',
    '#title' => t('Add workflow stamp description.'),
    '#description' => t('Add add a more detail description for a new workflow stamp'),
    '#default_value' => "",
    '#weight' => 4,
  );
  $options = array();
  $results = islandora_workflow_rest_get_all_stamps();
  while ($data = $results->fetchObject()) {
    $options[$data->stamp]['stamp'] = $data->stamp;
    $options[$data->stamp]['descr'] = $data->descr;
  }
  $header = array(
    'stamp' => t('Name'),
    'descr' => t('Description'),
  );
  $form['Stamps'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no stamps."),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}

/**
 * The islandora_workflow_rest_admin submit handler.
 *
 * @param array $form
 *   The current form being submitted.
 * @param array $form_state
 *   The current $form_state submission state.
 */
function islandora_workflow_rest_admin_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');
  // Check the form triggering_element to perform the correct action.
  switch ($form_state['triggering_element']['#value']) {
    case "Add":
      if ($form_state['input']['workflow_stamp_field'] != "") {
        $stamp = islandora_workflow_rest_get_stamp($form_state['input']['workflow_stamp_field']);
        if (!isset($stamp) || $stamp == "") {
          islandora_workflow_rest_set_stamp($form_state['input']['workflow_stamp_field'], $form_state['input']['workflow_stamp_desc']);
        }
      }
      break;

    case "Delete":
      foreach ($form_state['values']['Stamps'] as $list_identifier => $is_checked) {
        if ($is_checked) {
          islandora_workflow_rest_remove_stamp($list_identifier);
        }
      }
      break;
  }
  if (ISLANDORA_WORKFLOW_REST_SOLR_DEFAULT_FIELD != $form_state['input']['workflow_solr_field']) {
    variable_set('workflow_solr_field', $form_state['input']['workflow_solr_field']);
  }
}
