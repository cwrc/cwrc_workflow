<?php
/**
 * @file
 * File contains enums and constant validation.
 */

/**
 * Basic Enumeration functionality for Workflow Constants.
 * @author discoverygarden
 */
abstract class BasicEnum {
  protected static $constCache = NULL;

  /**
   * Return all constants via reflection.
   */
  protected static function getConstants() {
    $reflect = new ReflectionClass(get_called_class());
    self::$constCache = $reflect->getConstants();
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
   * Return all constants via reflection.
   */
  public static function getAllConstants() {
    $reflect = new ReflectionClass(get_called_class());
    self::$constCache = $reflect->getConstants();
    return self::$constCache;
  }

  /**
   * Confirms valid constant values.
   *
   * @param string $value
   *   The value to confirm as valid.
   */
  public static function isValidValue($value) {
    $values = array_values(self::getConstants());
    return in_array($value, $values, $strict = FALSE);
  }

  /**
   * Get value for constant.
   *
   * @param string $value
   *   The constant to retrieve the value for.
   *
   * @return string
   *   The constant value.
   */
  public static function getConstantValue($value) {
    $values = self::getConstants();
    return $values[strtoupper($value)];
  }
}

/**
 * Most common WorkflowItem related constants.
 * @author discoverygarden
 */
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

  // Define query constants.
  const CHECK_STAMP = "workflow_assigned_stamp_mt";
  const CHECK_CATEGORY = "workflow_activity_category_mt";

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

  const PROCESS = "process";
  const CWRC = "cwrc";
  const WORKFLOW = "workflow";
  const ACTIVITY = "activity";
  const ASSIGNED = "assigned";
  const NOTE = "note";
  const MESSAGE = "message";
}

/**
 * Assigned Category WorkflowItem constants.
 * @author discoverygarden
 */
abstract class AssignedCategory extends BasicEnum {
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
}
