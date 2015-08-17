<?php

/**
 * @file
 * Rule action to add a WORKFLOW datastream to an ingested object as part of 
 * the cwrc_workflow module.
 */

/**
 *   Implements hook_rules_action_info().
 */
function islandora_workflow_rest_rules_action_info()
{
    $cond = array();
    
    $cond['islandora_workflow_rest_rules_add'] = array(
        'label' => t('Add a WORKFLOW datastream to an ingested object'),
        'group' => t('CWRC Workflow'),
        'parameter' => array(
            'islandora_object' => array(
                'type' => 'islandora_object',
                'label' => t('Object'),
                'description' => t('A Tuque object for the Fedora object from which to load the datastream, as an entity.')
            )
        )
    );
    
    return $cond;
}

/**
 * Rules action callback; grab a datastream from an object.
 */
function islandora_workflow_rest_rules_add(AbstractObject $islandora_object)
{
    module_load_include('inc', 'islandora_workflow_rest', 'includes/cwrc_workflow');
    module_load_include('inc', 'islandora_workflow_rest', 'includes/workflow_item');
    module_load_include('inc', 'islandora_workflow_rest', 'includes/basic_enum');
    
    if (!$islandora_object['WORKFLOW']) {
        $workflow = CWRCWorkflowAPI::fromTemplate();
        CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);
    } else {
        $workflow = CWRCWorkflowAPI::fromDatastream($islandora_object['WORKFLOW']);
    }
    
    $activity_params = array(
        "category" => "created",
        "status" => "c",
        "stamp" => "cwrc:cre"
    );
    
    $assigned_params = array();
    
    $tool_id = array();
    
    $activity = new WorkflowItem(WorkflowConst::ACTIVITY, $activity_params);
    $assigned = new WorkflowItem(WorkflowConst::ASSIGNED, $assigned_params);
    $entry    = new WorkflowItem(WorkflowConst::WORKFLOW, $tool_id);
    
    // Get the last workflow step, if exists. 
    $last_workflow = $workflow->entryToArray();
    if ($last_workflow) {
        $last_wf_string = islandora_workflow_rest_prepare_workflow_string($last_workflow, "previous");
    } else {
        $last_wf_string = "";
    }
    
    $workflow->addWorkflowStep($islandora_object->id, $entry, $activity, $assigned);
    CWRCWorkflowAPI::updateDatastream($workflow, $islandora_object);

}
