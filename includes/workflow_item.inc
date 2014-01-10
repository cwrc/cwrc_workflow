<?php
/**
 * @file
 * Defines the WorkflowItem class.
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

    // TODO: $value can be dynamic, but can sometimes use static values.
    // The value should be validated or strictly enforced.
    foreach ($attributes as $key => $value) {
      if (!WorkflowConst::isValidName($key)) {
        throw new InvalidArgumentException("$key is not a valid attribute in any WorkflowItem node. value:  $value");
      }
    }

    // Lets make sure this is a valid WorkflowItem.
    if (!WorkflowConst::isValidName($name)) {
      throw new InvalidArgumentException("$name is not a valid WorkflowNode");
    }
  }
}
