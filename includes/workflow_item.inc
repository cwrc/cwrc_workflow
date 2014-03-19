<?php
/**
 * @file
 * Defines the WorkflowItem class.
 */

/**
 * Generic Workflow item helper with basic validation.
 *
 * @author discoverygarden
 */
class WorkflowItem {
  public $workName;
  public $typeAttributes;
  public $text;

  /**
   * Construct a new workflow item.
   *
   * @param string $name
   *   The name constant from the WorkflowConst Class.
   * @param array $attributes
   *   The array of attributes to add to the given node.
   * @param string $inner_text
   *   The inner text of the node, if any.
   *
   * @throws InvalidArgumentException
   */
  public function __construct($name, array $attributes, $inner_text = NULL) {
    $this->workName = $name;
    $this->typeAttributes = $attributes;
    $this->text = $inner_text;
    // Validate attributes, and stamp values.
    foreach ($attributes as $key => $value) {
      if ($key != WorkflowConst::BODY) {
        if (strtoupper($value) != "SELECT") {
          if ($key == WorkflowConst::STAMP) {
            // Validate the stamp value.
            $stamp = islandora_workflow_rest_get_stamp($value);
            if ($stamp == "") {
              // Stamp is not a pre approved stamp, throw exception.
              throw new Exception("$value is not a valid stamp attribute, supplied for $name");
            }
          }
          if ($key == WorkflowConst::CATEGORY) {
            // Confirm value is a valid category.
            $lower = strtolower($value);

            if (!WorkflowConst::isValidValue($lower)) {
              throw new Exception("$value is not a valid Category value.");
            }
          }
          if ($key == WorkflowConst::STATUS) {
            // Confirm $value is a valid category entry.
            if (!WorkflowConst::isValidValue($value)) {
              throw new Exception("$value is not a valid Status value.");
            }
          }
          if (!WorkflowConst::isValidName($key)) {
            throw new Exception("$key is not a valid attribute in any WorkflowItem node. value:  $value");
          }
        }
      }
    }
    // Lets make sure this is a valid WorkflowItem.
    if (!WorkflowConst::isValidName($name)) {
      throw new Exception("$name is not a valid WorkflowNode");
    }
    $this->validate();
    if ($name == WorkflowConst::ACTIVITY) {
      // An Activity requires: Category/Stamp/Status.
      if (!isset($this->typeAttributes[WorkflowConst::STAMP]) ||
          !isset($this->typeAttributes[WorkflowConst::CATEGORY]) ||
          !isset($this->typeAttributes[WorkflowConst::STATUS])) {
        throw new Exception("Category, Stamp and Status are required fields.");
      }
    }
    if ($name == WorkflowConst::ASSIGNED) {
      // An Assigned requires (if set at all): Category.
      $keys = array_keys($this->typeAttributes);
      if (count($keys) > 0 && (!in_array(WorkflowConst::CATEGORY, $keys))) {
        throw new Exception("Assigned entries require atleast a Category entry");
      }
    }
  }

  /**
   * Additional typeAttributes validation.
   */
  protected function validate() {
    foreach ($this->typeAttributes as $key => $value) {
      if (strtoupper($value) == "SELECT" || strlen($value) == 0) {
        unset($this->typeAttributes[$key]);
      }
    }
  }
}
