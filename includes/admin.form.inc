<?php
/**
 * @file
 * Admin form implementations.
 */

/**
 * Admin page for adding additional stamps.
 *
 * @param unknown $form
 *   The current form.
 * @param unknown $form_state
 *   The current form_state.
 *
 * @return array
 *   The form array.
 */
function islandora_workflow_rest_admin($form, &$form_state) {
  $form['workflow_stamp_fs']['workflow_stamp_field'] = array(
    '#type' => 'textfield',
    '#title' => t('Add additional stamps'),
    '#description' => t('Include additional stamp values (used when adding an activity via workflow API).'),
    '#default_value' => "",
    '#weight' => 3,
  );
  return $form;
}

/**
 * The islandora_workflow_rest_admin submit handler.
 *
 * @param unknown $form
 *   The current form being submitted.
 * @param unknown $form_state
 *   The current $form_state submission state.
 */
function islandora_workflow_rest_admin_submit($form, &$form_state) {
  // TODO: Create a drupal table, and store additional values there.
}
