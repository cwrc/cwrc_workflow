<?php

/**
 * @file
 * Forms for reports.
 */

/**
 * Form for reports.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 *
 * @return array
 *   Completed form
 */
function islandora_workflow_rest_reports_form($form, &$form_state) {
  global $user;
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/report_utilities');
  module_load_include('inc', 'islandora_solr', 'includes/admin');
  drupal_add_css(drupal_get_path('module', 'islandora_solr') . '/css/islandora_solr.admin.css');
  drupal_add_library('system', 'ui.dialog');
  drupal_add_js(drupal_get_path('module', 'islandora_solr') . '/js/islandora_solr.admin.js');
  // Add the custom CSS to align the roles checkboxes.
  drupal_add_css(
      drupal_get_path('module', 'islandora_workflow_rest') .
      '/css/islandora_workflow_rest_report_css.css'
  );
  $field_to_add = FALSE;

  if (isset($_GET['action']) && $_GET['action'] == 'clone') {
    drupal_set_message(t('The workflow report has been copied. Click "Save Report" to save the new report.'));
  }

  if (isset($form_state['triggering_element'])) {
    if ($form_state['triggering_element']['#name'] == 'cwrc-reports-add-field') {
      $new_term = $form_state['values']['available_solr_fields'];
      $weight = isset($form_state['values']['table']['table']['rows']) ? count($form_state['values']['table']['table']['rows']) + 1 : 1;
      $field_to_add = array(
        'remove_field' => 0,
        'display_label' => $new_term,
        'hyperlink' => 0,
        'weight' => $weight,
        'solr_field' => $new_term,
      );
    }
    if ($form_state['triggering_element']['#name'] == 'cwrc-reports-remove-selected') {
      $rows_to_remove = array();
      foreach ($form_state['values']['table']['table']['rows'] as $key => $row) {
        if ($row['remove_field'] == TRUE) {
          $rows_to_remove[] = $key;
        }
      }
      foreach ($rows_to_remove as $row_number) {
        unset($form_state['values']['table']['table']['rows'][$row_number]);
      }
    }
  }

  $title = "<h1></h1>";
  $heading = t('Create Workflow Report');
  $records = islandora_workflow_rest_get_all_records();
  if (isset($_GET['lid'])) {
    $form['lid'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['lid'],
    );
    $form['action'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['action'],
    );
    $chosen = $records[$_GET['lid']];
    $action = $_GET['action'];
    if ($action == 'edit') {
      $heading = t('Edit Workflow Report');
      $activity = t('Edit');
    }
    else {
      $heading = t('Clone Workflow Report');
      $activity = t('Clone');
      $chosen['name'] .= ' - ' . t("Cloned");
    }

    $title = "<h1> " . $activity . " " . $chosen['name'] . "</h1><a href = 'reports'>Create new report</a>";
  }

  $header = array(
    array('data' => t('Name'), 'field' => 'name'),
    array('data' => t('Project'), 'field' => 'project', 'sort' => 'asc'),
    array('data' => t('Created by'), 'field' => 'createdby'),
    array("data" => ' '),
    array("data" => ' '),
    array("data" => ' '),
  );

  $query = db_select('islandora_workflow_rest_reports', 'r');
  $query->fields('r');
  $table_sort = $query->extend('TableSort')->orderByHeader($header);
  $pager = $table_sort->extend('PagerDefault')->limit(10);
  $result = $pager->execute();

  $rows = array();
  foreach ($result as $record) {
    $view_roles = unserialize($record->viewroles);
    $edit_roles = unserialize($record->editroles);
    if (!count(array_intersect($view_roles, $user->roles)) && !user_access(ISLANDORA_WORKFLOW_REST_ISLANDORA_ENTITIES_ACCESS_ALL_REPORTS)) {
      continue;
    }
    $can_edit = count(array_intersect($edit_roles, $user->roles)) || user_access(ISLANDORA_WORKFLOW_REST_ISLANDORA_ENTITIES_ACCESS_ALL_REPORTS);
    $can_view = user_access(ISLANDORA_WORKFLOW_REST_ISLANDORA_ENTITIES_VIEW_REPORTS);
    $lid = $record->lid;
    $edit_link = $can_edit ? l(t('Edit'), 'islandora_workflow_rest/reports', array('query' => array('lid' => $lid, 'action' => 'edit'))) : '';
    $clone_link = $can_edit ? l(t('Clone'), 'islandora_workflow_rest/reports', array('query' => array('lid' => $lid, 'action' => 'clone'))) : '';
    $report_name = $record->name;
    $view_link = $can_view ? l($report_name, "islandora/search/" . $record->query, array('query' => array('workflow_report' => $lid, 'display' => 'cwrc_report'))) : $report_name;
    $rows[] = array(
      array('data' => $view_link),
      array('data' => $record->project),
      array('data' => $record->createdby),
      array('data' => $edit_link),
      array('data' => $clone_link),
      array('data' => $can_edit ? l(t('Delete'), "islandora_workflow_rest/delete_report/$lid/$report_name") : ''),
    );
  }
  $markup = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'sort-table')));
  $markup .= theme('pager');
  $form['reports'] = array(
    '#type' => 'markup',
    '#markup' => $markup,
  );
  // Define fieldsets.
  $form['workflow_reports_fs'] = array(
    '#type' => 'fieldset',
    '#title' => filter_xss($heading),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  // Define workflow_reports_fs form elements.
  $form['workflow_reports_fs']['report_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Report Name'),
    '#default_value' => isset($chosen['name']) ? $chosen['name'] : '',
    '#weight' => -2,
  );
  $form['workflow_reports_fs']['report_project'] = array(
    '#type' => 'textfield',
    '#title' => t('Report Project'),
    '#default_value' => isset($chosen['project']) ? $chosen['project'] : '',
    '#weight' => -1,
  );
  $form['workflow_reports_fs']['report_type'] = array(
    '#type' => 'select',
    '#title' => t('Report Type'),
    '#weight' => 0,
    '#options' => array(
      0 => t('Solr'),
    ),
  );

  $form['workflow_reports_fs']['workflow_permissions_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Permissions'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['workflow_reports_fs']['title'] = array(
    '#type' => 'markup',
    '#markup' => $title,
  );
  $form['workflow_reports_fs']['workflow_solr_fs'] = array(
    '#type' => 'fieldset',
    '#title' => t('Solr Report'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // 1: Form update using AJAX.
  if (isset($form_state['triggering_element'])) {

    unset($form_state['input']['terms']);
    // Check for add.
    if ($form_state['triggering_element']['#value'] == '+') {
      $form_state['values']['terms'][] = array();
    }
    // Check for remove.
    elseif ($form_state['triggering_element']['#value'] == '-') {
      $field = $form_state['triggering_element']['#field'];
      array_splice($form_state['values']['terms'], $field, 1);
    }
    $values = $form_state['values'];
  }
  else {
    if (isset($chosen)) {
      $values = array(
        'terms' => unserialize($chosen['terms']),
      );
    }
    else {
      $values = array(
        'terms' => array(''),
      );
    }
  }

  $terms = array(
    '#type' => 'markup',
    '#prefix' => '<div id="cwrc-workflow-terms">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );
  foreach ($values['terms'] as $i => $value) {
    $term = array(
      '#tree' => TRUE,
      '#prefix' => '<div>',
      '#suffix' => '</div>',
    );
    $term['field'] = array(
      '#title' => t('Field'),
      '#type' => 'select',
      '#default_value' => isset($value['field']) ? $value['field'] : 'dc.title',
      '#options' => islandora_solr_get_fields('search_fields'),
    );
    $term['search'] = array(
      '#title' => t('Search terms'),
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => isset($value['search']) ? $value['search'] : '',
    );
    // Used for when the user presses enter on the search field.
    $term['hidden_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Search'),
      '#attributes' => array('style' => 'visibility:hidden;position:fixed;top:-1000px;right:-1000px;'),
    );
    $term['add'] = array(
      '#type' => 'button',
      '#value' => '+',
      '#attributes' => array('title' => t('Add field')),
      '#name' => 'add-field-' . $i,
      '#ajax' => array(
        'callback' => 'islandora_workflow_rest_search_terms',
        'wrapper' => 'cwrc-workflow-terms',
        'method' => 'replace',
        'effect' => 'fade',
        'progress' => array('type' => 'none'),
      ),
    );
    if (count($values['terms']) > 1) {
      $term['remove'] = array(
        '#type' => 'button',
        '#field' => $i,
        '#value' => '-',
        '#attributes' => array('title' => t('Remove field')),
        '#name' => 'remove-field-' . $i,
        '#ajax' => array(
          'callback' => 'islandora_workflow_rest_search_terms',
          'wrapper' => 'cwrc-workflow-terms',
          'method' => 'replace',
          'effect' => 'fade',
          'progress' => array('type' => 'none'),
        ),
      );
      if ((variable_get('islandora_solr_search_boolean', 'user') == 'user') && ((count($values['terms']) - 1) != $i)) {
        $term['boolean'] = array(
          '#type' => 'select',
          '#prefix' => '<div>',
          '#suffix' => '</div>',
          '#default_value' => isset($value['boolean']) ? $value['boolean'] : 'AND',
          '#options' => array(
            'AND' => 'AND',
            'OR' => 'OR',
            'NOT' => 'NOT',
          ),
        );
      }
    }
    $terms[] = $term;
  }

  // Add terms.
  $form['workflow_reports_fs']['workflow_solr_fs']['terms'] = $terms;
  // Add controls.
  $form['workflow_reports_fs']['workflow_solr_fs']['controls'] = array(
    '#type' => 'markup',
    '#prefix' => '<div class="islandora-solr-advanced-controls">',
    '#suffix' => '</div>',
  );

  $roles = user_roles();
  $role_names = array_values($roles);
  $current_edit_roles = isset($chosen) ? unserialize($chosen['editroles']) : array();
  $current_view_roles = isset($chosen) ? unserialize($chosen['viewroles']) : array();

  $form['workflow_reports_fs']['workflow_permissions_fs']['edit_roles'] = array(
    '#type' => 'checkboxes',
    '#prefix' => '<div class="report-checkboxes">',
    '#options' => drupal_map_assoc($role_names),
    '#title' => t('Edit Report role(s)'),
    '#default_value' => $current_edit_roles,
    '#attributes' => array(
      'class' => array('workflow-edit-roles-check'),
    ),
  );
  $form['workflow_reports_fs']['workflow_permissions_fs']['view_roles'] = array(
    '#type' => 'checkboxes',
    '#suffix' => '</div>',
    '#options' => drupal_map_assoc($role_names),
    '#title' => t('View Report role(s)'),
    '#default_value' => $current_view_roles,
    '#attributes' => array(
      'class' => array(
        'workflow-view-roles-check',
      ),
    ),
  );
  $form['islandora_solr_metadata_fields'] = array(
    '#type' => 'fieldset',
    '#title' => 'Displayed Report Fields',
    '#collapsed' => TRUE,
    '#collapsible' => TRUE,
  );
  // If there are values in the form_state use them for persistence in case of
  // AJAX callbacks, otherwise grab fresh values from the database.
  if (isset($form_state['values'])) {
    if (isset($form_state['values']['table'])) {
      $data = $form_state['values']['table']['table']['rows'];
    }
  }
  else {
    $data = isset($chosen) ? unserialize($chosen['solr_terms']) : islandora_workflow_rest_cwrc_forkflow_prep_fields();
  }

  if ($field_to_add !== FALSE) {
    $data[] = $field_to_add;
  }

  $form['islandora_solr_metadata_fields']['search_term_table_wrapper'] = array(
    '#prefix' => '<div id="cwrc-workflow-fields-wrapper">',
    '#suffix' => '</div>',
  );
  $form['islandora_solr_metadata_fields']['search_term_table_wrapper']['table'] = islandora_workflow_rest_cwrc_workflow_table_build($data);
  if (count($form['islandora_solr_metadata_fields']['search_term_table_wrapper']['table']['table']['rows'])) {
    $form['islandora_solr_metadata_fields']['search_term_table_wrapper']['remove_selected'] = array(
      '#type' => 'button',
      '#value' => t('Remove selected field from report'),
      '#name' => 'cwrc-reports-remove-selected',
      '#ajax' => array(
        'callback' => 'islandora_workflow_rest_cwrc_workflow_fields_ajax',
        'wrapper' => 'cwrc-workflow-fields-wrapper',
      ),
    );
  }
  $form['islandora_solr_metadata_fields']['search_term_table_wrapper']['add_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add field'),
    '#collapsed' => TRUE,
    '#collapsible' => FALSE,
  );
  $form['islandora_solr_metadata_fields']['search_term_table_wrapper']['add_fieldset']['available_solr_fields'] = array(
    '#type' => 'textfield',
    '#description' => t('A field from within Solr'),
    '#size' => 105,
    '#autocomplete_path' => 'islandora_solr/autocomplete_luke',
    '#default_value' => '',
  );
  $form['islandora_solr_metadata_fields']['search_term_table_wrapper']['add_fieldset']['add_field'] = array(
    '#type' => 'button',
    '#value' => t('Add'),
    "#executes_submit_callback" => FALSE,
    '#name' => 'cwrc-reports-add-field',
    '#ajax' => array(
      'callback' => 'islandora_workflow_rest_cwrc_workflow_fields_ajax',
      'wrapper' => 'cwrc-workflow-fields-wrapper',
    ),
  );
  $form['submit'][] = array(
    '#type' => 'submit',
    '#value' => t('Save Report'),
  );
  $form['search_form'] = theme('islandora_solr_advanced_search_form');
  return $form;
}

/**
 * Validator for report form.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 */
function islandora_workflow_rest_reports_form_validate($form, &$form_state) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/report_utilities');
  drupal_add_css(
      drupal_get_path('module', 'islandora_workflow_rest') .
      '/css/islandora_workflow_rest_report_css.css'
  );
  if ($form_state['triggering_element']['#value'] != 'Save Report') {
    return;
  }
  $name = $form_state['values']['report_name'];
  if ($name == '') {
    form_set_error('report_name', t('Name cannot be blank.'));
  }
  $record = islandora_workflow_rest_get_record('name', $name);
  $edit = isset($_GET['action']) && $_GET['action'] == 'edit';
  if ($record && !$edit) {
    // This is a new report, with the name of an existing one.
    form_set_error('report_name', t('This name is already in use.  Please select another.'));
  }
  if ($form_state['values']['report_project'] == '') {
    form_set_error('report_project', t('Project name cannot be blank.'));
  }
  if (count(array_filter($form_state['values']['edit_roles'])) == 0) {
    form_set_error('edit_roles', t('Please choose at least one edit role.'));
  }
  if (count(array_filter($form_state['values']['view_roles'])) == 0) {
    form_set_error('view_roles', t('Please choose at least one view role.'));
  }
  $has_reports = isset($form_state['values']['table']['table']['rows']);
  $has_report = $has_reports && count($form_state['values']['table']['table']['rows']);
  if (!$has_reports || !$has_report) {
    form_set_error('add_field', t('Please choose at least one field to display.'));
  }
}

/**
 * Submit function.
 *
 * @global type $user
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal from state
 */
function islandora_workflow_rest_reports_form_submit($form, &$form_state) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/report_utilities');
  global $user;
  $record = array();
  $record['name'] = $form_state['values']['report_name'];
  $record['project'] = $form_state['values']['report_project'];
  $record['createdby'] = !user_is_anonymous() ? $user->name : t("unknown");
  $record['editroles'] = serialize(array_filter($form_state['values']['edit_roles']));
  $record['viewroles'] = serialize(array_filter($form_state['values']['view_roles']));
  $record['query'] = islandora_workflow_rest_build_query($form_state['values']['terms']);
  $record['terms'] = serialize($form_state['values']['terms']);
  $record['solr_terms'] = serialize($form_state['values']['table']['table']['rows']);

  if (isset($_GET['action']) && $_GET['action'] == 'edit') {
    islandora_workflow_rest_update_record($record, $_GET['lid']);
  }
  else {
    islandora_workflow_rest_add_record($record);
  }
  $form_state['redirect'] = 'islandora_workflow_rest/reports';
}

/**
 * AJAX callback for report form.
 *
 * @param array $form
 *   An associative array containing form structure.
 * @param array $form_state
 *   An associative array containing form state.
 *
 * @return array
 *   An associative array containing the fully built form element.
 */
function islandora_workflow_rest_search_terms($form, &$form_state) {
  return $form['workflow_reports_fs']['workflow_solr_fs']['terms'];
}

/**
 * Builds query from terms.
 *
 * @param array $terms
 *   Solr terms array as defines in islandora solr search
 *
 * @return string
 *   Solr query
 */
function islandora_workflow_rest_build_query($terms) {
  // Collect query values.
  $query_array = array();
  foreach ($terms as $term) {
    $field = $term['field'];
    $search = trim($term['search']);
    $boolean = (isset($term['boolean'])) ? $term['boolean'] : variable_get('islandora_solr_search_boolean', 'user');
    if (!empty($search)) {
      $query_array[] = array(
        'search' => "$field:($search)",
        'boolean' => $boolean,
      );
    }
  }
  $query = '';
  $iterator = 0;
  foreach ($query_array as $term) {
    $query .= $term['search'];
    if (count($query_array) - 1 != $iterator) {
      $query .= ' ' . $term['boolean'] . ' ';
    }
    $iterator++;
  }
  if (empty($query)) {
    $query = '*:*';
  }
  return $query;
}

/**
 * The workflow skip entries confirmation form.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $lid
 *   Lid to be deleted.
 * @param string $report_name
 *   Name of report to be deleted
 *
 * @return array
 *   The drupal form definition.
 */
function islandora_workflow_rest_cwrc_workflow_delete_reports_form(array $form, array &$form_state, $lid, $report_name = 'unnamed') {
  $form_state['lid'] = $lid;
  return confirm_form($form, t('Are you sure you want to delete @report', array('@report' => $report_name)), "islandora_workflow_rest/reports", t('This action cannot be undone.'), t('Delete report'), t('Cancel')
  );
}

/**
 * Marks checked log entries as skipped.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_workflow_rest_cwrc_workflow_delete_reports_form_submit(array $form, array &$form_state) {
  $lid = intval($form_state['lid']);
  db_delete('islandora_workflow_rest_reports')
      ->condition('lid', $lid)
      ->execute();
  drupal_set_message(t("Report deleted"));
  drupal_goto('islandora_workflow_rest/reports');
}

/**
 * Helper function to construct the fields tableselect.
 *
 * @param array $data
 *   An array of data representing rows to be rendered in the tableselect.
 *
 * @return array
 *   An array representing the tableselect to be rendered.
 */
function islandora_workflow_rest_cwrc_workflow_table_build($data) {
  $form = array();
  $rows = array();
  if ($data) {
    uasort($data, 'drupal_sort_weight');
    $delta = count($data);
    $weight = 1;
    $map = function($field) use($delta, &$weight) {
      $value = $weight;
      $weight++;
      if (!$field) {
        return;
      }
      return array(
        'remove_field' => array(
          '#type' => 'checkbox',
          '#default_value' => isset($field['remove']) ? $field['remove'] : FALSE,
        ),
        'solr_field_markup' => array('#markup' => filter_xss($field['solr_field'])),
        'display_label' => array(
          '#type' => 'textfield',
          '#default_value' => isset($field['display_label']) ? filter_xss($field['display_label']) : '',
        ),
        'hyperlink' => array(
          '#type' => 'checkbox',
          '#default_value' => isset($field['hyperlink']) ? $field['hyperlink'] : FALSE,
        ),
        'weight' => array(
          '#type' => 'weight',
          '#title' => t('Weight'),
          '#default_value' => $value,
          '#delta' => $delta,
        ),
        'solr_field' => array(
          '#type' => 'value',
          '#value' => filter_xss($field['solr_field']),
        ),
      );
    };
    $rows = array_filter(array_map($map, $data));
  }
  // Add the table to the form.
  $form['#tree'] = TRUE;
  $form['table'] = array(
    '#type' => 'markup',
    '#header' => array(
      t('Remove'),
      t('Solr Field'),
      t('Display Label'),
      t('Hyperlink'),
      '',
    ),
    'rows' => $rows,
    '#theme' => 'islandora_solr_metadata_management_table',
    '#empty' => t('No fields associated'),
  );
  return $form;
}

/**
 * AJAX callback.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array representing the Drupal form state.
 *
 * @return array
 *   An array representing the portion of the form we are rendering.
 */
function islandora_workflow_rest_cwrc_workflow_fields_ajax(&$form, &$form_state) {
  return $form['islandora_solr_metadata_fields']['search_term_table_wrapper'];
}

/**
 * Gets default solr fields and converts them to table rows.
 *
 * @return array
 *   Associative array of row values for table
 */
function islandora_workflow_rest_cwrc_forkflow_prep_fields() {
  $default_fields = islandora_solr_get_fields('result_fields', FALSE, FALSE);
  $solr_fields = array();
  foreach ($default_fields as $field) {
    $solr_fields[] = array(
      'remove_field' => 0,
      'display_label' => $field['solr_field_settings']['label'],
      'hyperlink' => $field['solr_field_settings']['link_to_object'],
      'weight' => $field['weight'],
      'solr_field' => $field['solr_field'],
    );
  }
  return $solr_fields;
}
