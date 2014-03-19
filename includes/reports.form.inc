<?php 


function islandora_workflow_rest_reports_form($form, &$form_state) {
  // Add the custom CSS to align the roles checkboxes.
  drupal_add_css(
    drupal_get_path('module', 'islandora_workflow_rest') .
    '/css/islandora_workflow_rest_report_css.css'
  );
//   $something = islandora_solr_advanced_search_form($form, $form_state);
//   $shit = drupal_render($something);
//  // $stuff = drupal_get_form('islandora_solr_advanced_search_form');
//   dsm($form['workflow_reports_fs']['skskkdndnd'], "stuff");
//   $form['contact_information'] = array(
//     '#markup' => $shit,
//   );
  // Define fieldsets.
  $form['workflow_reports_fs'] = array(
      '#type' => 'fieldset',
      '#title' => t('Create Workflow Reports'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
  );
  $form['workflow_reports_fs']['workflow_permissions_fs'] = array(
      '#type' => 'fieldset',
      '#title' => t('Permissions'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
  );
  $form['workflow_reports_fs']['workflow_solr_fs'] = array(
      '#type' => 'fieldset',
      '#title' => t('Solr Report'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
  );

  // Construct our table of reports.
  $options = array();
  $item_checked = array();
  $header = array(
      'pid' => t('PID'),
      'label' => t('Label'),
      'dsid' => t('Datastream ID'),
      'delay' => t('Logging Delay'),
  );
  $form['content_model_table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => array(),
      '#default_value' => array(),
      '#attributes' => '',
      '#empty' => t("There are no content models??"),
  );

  // Define workflow_reports_fs form elements.
  $form['workflow_reports_fs']['report_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Report Name'),
      '#default_value' => "",
      '#required' => TRUE,
  );
  $form['workflow_reports_fs']['report_project'] = array(
      '#type' => 'textfield',
      '#title' => t('Report Project'),
      '#default_value' => "",
      '#required' => TRUE,
  );
  $form['workflow_reports_fs']['report_type'] = array(
      '#type' => 'select',
      '#title' => t('Report Type'),
      '#options' => array(
          0 => t('Solr'),
      ),
  );
  
  // workflow_permissions_fs form elements.
  // Create our roles checkboxes (Edit and View)
  // TODO: Change to be available on permission.
  $roles = user_roles();
  $role_names = array_values($roles);
  $form['workflow_reports_fs']['workflow_permissions_fs']['edit_roles'] = array(
      '#type' => 'checkboxes',
      '#prefix' => '<div class="report-checkboxes">',
      '#options' => drupal_map_assoc($role_names),
      '#title' => t('Edit Report role(s)'),
      '#attributes' => array(
        'class' => array('workflow-edit-roles-check')
      )
  );
  $form['workflow_reports_fs']['workflow_permissions_fs']['view_roles'] = array(
      '#type' => 'checkboxes',
      '#suffix' => '</div>',
      '#options' => drupal_map_assoc($role_names),
      '#title' => t('View Report role(s)'),
      '#attributes' => array(
          'class' => array(
              'workflow-view-roles-check'
          )
      )
  );
  $form['submit'][] = array(
    '#type' => 'submit',
    '#value' => t('Save Report'),
  );
  $form['search_form'] = theme('islandora_solr_advanced_search_form');
  return $form;
}

function islandora_workflow_rest_reports_form_submit($form, &$form_state) {
  dsm(array(), "hello there submit");
}