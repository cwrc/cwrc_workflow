<?php
/**
 * @file
 * Page callbacks for islandora_workflow_rest.
 */

/**
 * Test menu callback.
 */
function islandora_workflow_rest_add_workflow() {
  module_load_include('inc', 'php_lib', 'DOMHelpers');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/cwrc_workflow');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/basic_enum');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/workflow_item');
  $module_path = drupal_get_path('module', 'islandora_workflow_rest');

  $islandora_object = islandora_object_load("islandora:9");
  if (!$islandora_object['WORKFLOW']) {
    $workflow = CWRCWorkflowAPI::fromTemplate();
    CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);
  }
  else {
    $workflow = CWRCWorkflowAPI::fromDatastream($islandora_object['WORKFLOW']);
  }
  // Construct the workflow elements.
  $activity = new WorkflowItem(WorkflowConst::ACTIVITY,
    array(
      'category' => 'sample_category',
      'stamp' => 'sample_stamp',
      'status' => 'sample_status',
    ),
    "This is the body of the note element"
  );
  $assigned = new WorkflowItem(WorkflowConst::ASSIGNED,
    array(
      "category" => "content_contribution",
      "recipient" => "Me",
      "subject" => "Test Subject",
      "body" => "This is a test entry. Additonal text required.",
    )
  );
  $entry = new WorkflowItem(WorkflowConst::WORKFLOW,
    array(
      'toolID' => '',
      'workflowID' => '',
    )
  );
  $workflow->addWorkflowStep($islandora_object->id, $entry, $activity, $assigned);
  CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);
  dsm($islandora_object["WORKFLOW"]->content, "updated datastream");
  return json_encode("Test");
}
