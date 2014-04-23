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
  $form['manage_workflow']['#type'] = 'vertical_tabs';

  $form['manage_workflow']['workflow_stamp_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Stamp'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['manage_workflow']['workflow_query_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure SOLR Query'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['manage_workflow']['workflow_cm_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configure Event Logging'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['manage_workflow']['workflow_ns_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Namespace Management'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['manage_workflow']['workflow_si_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Server Identifier'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['manage_workflow']['workflow_si_fs']['workflow_si_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Set the server identifier.'),
    '#description' => t('This value is added to user name in a workflow log.'),
    '#default_value' => variable_get("islandora_workflow_rest_server_identifier", ""),
  );
  $form['manage_workflow']['workflow_si_fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Server Identifier'),
  );
  $form['manage_workflow']['workflow_ns_fs']['manage_namespace'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Stamp Namespace Restrictions.'),
    '#description' => t('When enabled, only stamps whos namespace matches that of the give
      object will be available in workflow entries.'),
    '#default_value' => variable_get('islandora_workflow_rest_enable_namespace', 0),
  );
  $form['manage_workflow']['workflow_ns_fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Set'),
  );
  $form['manage_workflow']['workflow_query_fs']['workflow_solr_field'] = array(
    '#type' => 'textfield',
    '#title' => t('SOLR Query Field'),
    '#description' => t('Configure the SOLR query Workflow will use with the rest interface.'),
    '#size' => 45,
    '#autocomplete_path' => 'islandora_workflow_rest/autocomplete_luke',
    '#default_value' => variable_get('workflow_solr_field', ISLANDORA_WORKFLOW_REST_SOLR_DEFAULT_FIELD),
  );
  $form['manage_workflow']['workflow_query_fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update SOLR Field'),
  );
  $form['manage_workflow']['workflow_stamp_fs']['workflow_stamp_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Add additional stamps'),
    '#description' => t('Include additional stamp values (used when adding an activity via workflow API).'),
    '#default_value' => "",
  );
  $form['manage_workflow']['workflow_stamp_fs']['workflow_stamp_desc'] = array(
    '#type' => 'textfield',
    '#title' => t('Add workflow stamp description.'),
    '#description' => t('Add add a more detail description for a new workflow stamp'),
    '#default_value' => "",
  );
  $options = array();
  $results = islandora_workflow_rest_get_all_stamps(10);
  while ($data = $results->fetchObject()) {
    $options[$data->stamp]['stamp'] = $data->stamp;
    $options[$data->stamp]['descr'] = $data->descr;
  }
  $header = array(
    'stamp' => t('Name'),
    'descr' => t('Description'),
  );
  $form['manage_workflow']['workflow_stamp_fs']['Stamps'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array(),
    '#empty' => t("There are no stamps."),
  );
  $form['manage_workflow']['workflow_stamp_fs']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  $form['manage_workflow']['workflow_stamp_fs']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['manage_workflow']['workflow_stamp_fs']['pager'] = array('#markup' => theme('pager'));

  // Add our content models to monitor on change.
  $form['manage_workflow']['workflow_cm_fs']['content_models'] = islandora_workflow_rest_admin_cm_table_form();
  $form['manage_workflow']['workflow_cm_fs']['content_models']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
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
  $drupal_message = "";
  // Check the form triggering_element to perform the correct action.
  switch ($form_state['triggering_element']['#value']) {
    case "Add":
      if ($form_state['input']['workflow_stamp_field'] != "") {
        $stamp = islandora_workflow_rest_get_stamp($form_state['input']['workflow_stamp_field']);
        if (!isset($stamp) || $stamp == "") {
          $stamp_field = $form_state['input']['workflow_stamp_field'];
          $stamp_desc = $form_state['input']['workflow_stamp_desc'];
          islandora_workflow_rest_set_stamp($stamp_field, $stamp_desc);
          $drupal_message = t("Stamp @stamp has been added.", array('@stamp' => $stamp_field));
        }
      }
      break;

    case "Delete":
      foreach ($form_state['values']['Stamps'] as $list_identifier => $is_checked) {
        if ($is_checked) {
          islandora_workflow_rest_remove_stamp($list_identifier);
          drupal_set_message(
            t("@msg",
              array(
                '@msg' => t("Stamp @lid has been removed.", array('@lid' => $list_identifier)),
              )
            ),
          'status'
          );
        }
      }
      break;

    case "Update":
      module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
      $content_models = islandora_get_content_models();
      $notify = FALSE;
      foreach ($form_state['values']['content_model_table'] as $list_identifier => $is_checked) {
        $entry_is_checked = ($is_checked ? 1 : 0);
        // Remove all entries, update to match the form exactly.
        islandora_workflow_rest_delete_ds_logs_admin($list_identifier);
        // Update db table to match.
        if ($entry_is_checked) {
          $notify = TRUE;
          $dsid = $form_state['input'][$list_identifier]['dsid_entry'];
          islandora_workflow_rest_set_ds_log_admin(
            $form_state['input'][$list_identifier]['dsid_entry'],
            $form_state['input'][$list_identifier]['delay_entry'],
            $list_identifier,
            $content_models[$list_identifier]['label']
          );
          drupal_set_message(
            t("@msg",
              array(
                '@msg' => t("Content model @lid is monitoring datastream @dsid.", array('@lid' => $list_identifier, '@dsid' => $dsid)),
              )
            ),
          'status'
          );
        }
      }
      if ($notify == FALSE) {
        drupal_set_message(
          t("@msg",
            array(
              '@msg' => t("No rows affected. Select all content models to monitor, and click update."),
            )
          ),
          'status'
        );
      }
      break;

    case "Set":
      $drupal_message = t("Namespace requirements disabled.");
      if ($form_state['values']['manage_namespace']) {
        $drupal_message = t("Namespace requirements enabled.");
      }
      variable_set('islandora_workflow_rest_enable_namespace', $form_state['values']['manage_namespace']);
      break;

    case "Update Server Identifier":
      $server_id = $form_state['values']['workflow_si_field'];
      variable_set('islandora_workflow_rest_server_identifier', $server_id);
      $drupal_message = t("Server Identifier is now @server_id.", array('@server_id' => $server_id));
      break;

    case "Update SOLR Field":
      $solr_field = $form_state['values']['workflow_solr_field'];
      variable_set('workflow_solr_field', $solr_field);
      $drupal_message = t("Solr Field is now @solr_field", array('@solr_field' => $solr_field));
      break;
  }
  drupal_set_message(
    t("@msg",
      array(
        '@msg' => $drupal_message,
      )
    ),
    'status'
  );
}
