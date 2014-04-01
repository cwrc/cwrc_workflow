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
  // Add the custom CSS to align the roles checkboxes.
  drupal_add_css(
      drupal_get_path('module', 'islandora_workflow_rest') .
      '/css/islandora_workflow_rest_report_css.css'
  );
  $title = "<h1></h1>";
  $heading = t('Create Workflow Report');
  $records = islandora_workflow_rest_get_all_records();
  if (isset($_GET['lid'])) {
    $chosen = $records[$_GET['lid']];
    $title = "<h1> " . t('Edit') . " " . $chosen['name'] . "</h1><a href = 'reports'>Create new report</a>";
    $heading = t('Edit Workflow Report');
    $form['lid'] = array(
      '#type' => 'hidden',
      '#value' => $_GET['lid'],
    );
  }

  $header = array(
    array('data' => t('Name'), 'field' => 'name'),
    array('data' => t('Project'), 'field' => 'project', 'sort' => 'asc'),
    array('data' => t('Created by'), 'field' => 'createdby'),
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
    $edit_link = $can_edit ? l(t('Edit'), 'islandora_workflow_rest/reports', array('query' => array('lid' => $lid))) : t('Edit');
    $report_name = $record->name;
    $view_link = $can_view ? l($report_name, "islandora/search/" . $record->query, array('query' => array('workflow_report' => $lid))) : $report_name;
    $rows[] = array(
      array('data' => $view_link),
      array('data' => $record->project),
      array('data' => $record->createdby),
      array('data' => $edit_link),
      array('data' => $can_edit ? l(t('Delete'), "islandora_workflow_rest/delete_report/$lid/$report_name") : t('Delete')),
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
    '#required' => TRUE,
    '#default_value' => isset($chosen['name']) ? $chosen['name'] : '',
    '#weight' => -2,
  );
  $form['workflow_reports_fs']['report_project'] = array(
    '#type' => 'textfield',
    '#title' => t('Report Project'),
    '#default_value' => isset($chosen['project']) ? $chosen['project'] : '',
    '#required' => TRUE,
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
  if ($form_state['triggering_element']['#value'] == '+' || $form_state['triggering_element']['#value'] == '-') {
    return;
  }
  $name = $form_state['values']['report_name'];
  if ($name == '') {
    form_set_error('report_name', t('Name cannot be blank.'));
  }
  $record = islandora_workflow_rest_get_record('name', $name);
  if ($record && !isset($_GET['lid'])) {
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

  // This may seam silly, but is necessary as the
  // SOLR AJAX request (adding additional solr fields)
  // strips the 'lid' field from
  // the form_state, prior to true form submission.
  if (isset($_GET['lid'])) {
    $form_state['values']['lid'] = $_GET['lid'];
    $form_state['rebuild'] = TRUE;
  };
  if (isset($form_state['values']['lid'])) {
    islandora_workflow_rest_update_record($record, $form_state['values']['lid']);
  }
  else {
    islandora_workflow_rest_add_record($record);
  }
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
function cwrc_workflow_delete_reports_form(array $form, array &$form_state, $lid, $report_name) {
  $form_state['lid'] = $lid;
  return confirm_form($form, t('Are you sure you want to delete @report', array('@report' => $report_name)), "islandora_workflow_rest/reports", t('This action cannot be undone.'), t('Delete report'), t('Cancel')
  );
}

/**
 * Marks checked log entrieds as skipped.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function cwrc_workflow_delete_reports_form_submit(array $form, array &$form_state) {
  $lid = intval($form_state['lid']);
  db_delete('islandora_workflow_rest_reports')
      ->condition('lid', $lid)
      ->execute();
  drupal_set_message(t("Report deleted"));
  drupal_goto('islandora_workflow_rest/reports');
}
