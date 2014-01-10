<?php
/**
 * @file
 * File contains as enums and constant validation.
 */

abstract class BasicEnum {
  protected static $constCache = NULL;

  /**
   * Return all constants via reflection.
   */
  protected static function getConstants() {
    if (self::$constCache === NULL) {
      $reflect = new ReflectionClass(get_called_class());
      self::$constCache = $reflect->getConstants();
    }
    return self::$constCache;
  }

  /**
   * Checks if a given value is a valid constant.
   *
   * @param string $name
   *   The constant value.
   * @param bool $strict
   *   Enforce strict values.
   */
  public static function isValidName($name, $strict = FALSE) {
    $constants = self::getConstants();
    if ($strict) {
      return array_key_exists($name, $constants);
    }
    $keys = array_map('strtolower', array_keys($constants));
    return in_array(strtolower($name), $keys);
  }

  /**
   * Confirms valid constant values.
   *
   * @param string $value
   *   The value to confirm as valid.
   */
  public static function isValidValue($value) {
    $values = array_values(self::getConstants());
    return in_array($value, $values, $strict = TRUE);
  }
}

abstract class WorkflowConst extends BasicEnum {
  // Workflow category.
  const CREATED = "created";
  const DEPOSITED = "deposited";
  const METADATA_CONTRIBUTION = "metadata_contribution";
  const CONTENT_CONTRIBUTION = "content_contribution";
  const CHECKED = "checked";
  const MACHINE_PROCESSED = "machine_processed";
  const USER_TAGGED = "user-tagged";
  const RIGHTS_ASSIGNED = "rights_assigned";
  const PUBLISHED = "published";
  const PEER_REVIEWED_EVALUATED = "peer-reviewed/evaluated";
  const WITHDRAWN = "withdrawn";
  const DELETED = "deleted";

  // Workflow status.
  const C = "c";
  const IPR = "ipr";
  const P = "p";
  const I = "i";

  // WorkflowNode Attributes.
  const CATEGORY = "category";
  const STATUS = "status";
  const STAMP = "stamp";
  const TOOLID = "toolID";
  const TIME = "time";
  const DATE = "date";
  const USERID = "userID";
  const WORKFLOWID = "workflowID";
  const RECIPIENT = "recipient";
  const SUBJECT = "subject";
  const BODY = "body";

  const CWRC = "cwrc";
  const WORKFLOW = "workflow";
  const ACTIVITY = "activity";
  const ASSIGNED = "assigned";
  const NOTE = "note";
  const MESSAGE = "message";
}
