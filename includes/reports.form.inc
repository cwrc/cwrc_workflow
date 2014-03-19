<?php 


function islandora_workflow_rest_reports_form($form, &$form_state) {
	drupal_add_css(drupal_get_path('module', 'islandora_workflow_rest') . '/css/islandora_workflow_rest_report_css.css');
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
			'#size' => 60,
			'#maxlength' => 128,
			'#required' => TRUE,
	);
	$form['workflow_reports_fs']['report_project'] = array(
			'#type' => 'textfield',
			'#title' => t('Report Project'),
			'#default_value' => "",
			'#size' => 60,
			'#maxlength' => 128,
			'#required' => TRUE,
	);
	$form['workflow_reports_fs']['report_type'] = array(
			'#type' => 'select',
			'#title' => t('Report Type'),
			'#options' => array(
					0 => t('Solr'),
			),
	);
	
	
	$roles = user_roles();
	dsm($roles, "rols");
	$role_names = array_values($roles);
	
	$form['workflow_permissions_fs']['edit_roles'] = array(
			'#type' => 'checkboxes',
			'#prefix' => '<div class="report-checkboxes">',
			'#options' => drupal_map_assoc($role_names),
			'#title' => t('Edit Report role(s)'),
			'#attributes' => array(
				'class' => array('workflow-edit-roles-check')
			)
	);
	$form['workflow_permissions_fs']['view_roles'] = array(
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
	// Define workflow_permissions_fs form elements.
	
	
// 	foreach ($roles as $role) {
// 		$safe_role = str_replace(" ", "_", $role);
// 		$safe_role .= "_edit";
// 		$form['workflow_permissions_fs'][$safe_role] = array(
// 			'#type' => 'checkbox',
// 			'#title' => t("@role", array('@role' => $role)),
// 		);
// 	}
	
// 	foreach ($roles as $role) {
// 		$safe_role = str_replace(" ", "_", $role);
// 		$safe_role .= "_view";
// 		$form['workflow_permissions_fs'][$safe_role] = array(
// 				'#type' => 'checkbox',
// 				'#title' => t("@role", array('@role' => $role)),
// 		);
// 	}
	
	
	
	//  $data = islandora_workflow_rest_get_all_ds_logs_admin(1000);
//   $options = array();
//   $item_checked = array();
  // All stored content models.
//   while ($content_model = $data->fetchObject()) {
//     $monitor_datastream = array(
//       '#type' => 'textfield',
//       '#value' => $content_model->dsid,
//       '#title' => 'Datastream ID',
//       '#title_display' => 'invisible',
//       '#name' => $content_model->pid . '[dsid_entry]',
//     );
//     $logging_delay = array(
//       '#type' => 'textfield',
//       '#value' => $content_model->delay,
//       '#title' => 'Logging Delay',
//       '#title_display' => 'invisible',
//       '#name' => $content_model->pid . '[delay_entry]',
//     );
//     $options[$content_model->pid]['pid'] = $content_model->pid;
//     $options[$content_model->pid]['label'] = $content_model->label;
//     $options[$content_model->pid]['dsid'] = array('data' => $monitor_datastream);
//     $options[$content_model->pid]['delay'] = array('data' => $logging_delay);
//     $item_checked[$content_model->pid] = 1;
//   }
//   $set_keys = array_keys($options);
//   foreach ($options as $ckey => $value) {
//     unset($content_models[$ckey]);
//   }
//   foreach ($content_models as $key => $value) {
//     // Add any additional content models as available.
//     $monitor_datastream = array(
//       '#type' => 'textfield',
//       '#value' => 'TN',
//       '#title' => 'Datastream ID',
//       '#title_display' => 'invisible',
//       '#name' => $key . '[dsid_entry]',
//     );
//     $logging_delay = array(
//       '#type' => 'textfield',
//       '#value' => '30',
//       '#title' => 'Logging Delay',
//       '#title_display' => 'invisible',
//       '#name' => $key . '[delay_entry]',
//     );
//     $options[$key]['pid'] = $key;
//     $options[$key]['label'] = $value['label'];
//     $options[$key]['dsid'] = array('data' => $monitor_datastream);
//     $options[$key]['delay'] = array('data' => $logging_delay);
//     $item_checked[$key] = 0;
//   }
//   $header = array(
//     'pid' => t('PID'),
//     'label' => t('Label'),
//     'dsid' => t('Datastream ID'),
//     'delay' => t('Logging Delay'),
//   );
//   $form['content_model_table'] = array(
//     '#type' => 'tableselect',
//     '#header' => $header,
//     '#options' => array(),
//     '#default_value' => array(),
//     '#attributes' => '',
//     '#empty' => t("There are no content models??"),
//   );
	return $form;
}

function islandora_workflow_rest_reports_form_submit($form, &$form_state) {
	dsm(array(), "hello there submit");
}