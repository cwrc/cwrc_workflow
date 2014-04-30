<?php

/**
 * @file
 * Holds helper functions for islandora_workflow_rest.
 */

/**
 * Set a new stamp.
 *
 * @param string $stamp
 *   The stamp to persist.
 */
function islandora_workflow_rest_set_stamp($stamp, $descr = "") {
  db_insert('islandora_workflow_rest_stamps')
      ->fields(array(
        'stamp' => $stamp,
        'descr' => $descr,
      ))
      ->execute();
}

/**
 * Remove an object's embargo information.
 *
 * @param string $stamp
 *   The stamp to remove.
 */
function islandora_workflow_rest_remove_stamp($stamp) {
  db_delete('islandora_workflow_rest_stamps')
      ->condition('stamp', $stamp)
      ->execute();
}

/**
 * Get a stamp by stamp name.
 *
 * @param string $stamp
 *   The stamp to check for/get.
 *
 * @return string
 *   The human readable stamp name.
 */
function islandora_workflow_rest_get_stamp($stamp) {
  $list = db_select('islandora_workflow_rest_stamps', 'c')
      ->fields('c')
      ->condition('stamp', $stamp)
      ->execute();
  $data = $list->fetchObject();
  $return_info = (isset($data->stamp) ? $data->stamp : "");
  return $return_info;
}

/**
 * Retrieve all stamp objects.
 *
 * @param int $limit
 *   The limit to set on the query.
 *
 * @return array
 *   The list of workflow stamps.
 */
function islandora_workflow_rest_get_all_stamps($limit = 10) {
  $query = db_select('islandora_workflow_rest_stamps', 'r');
  $query->fields('r', array("lid", "stamp", "descr"));
  $table_sort = $query->extend('TableSort');
  $pager = $table_sort->extend('PagerDefault')->limit($limit);
  $stamps = $pager->execute();
  return $stamps;
}

/**
 * Set a new Content Model log entry.
 *
 * @param string $type
 *   The type of edit (EX: add, delete...).
 * @param string $user
 *   The user making the modification.
 * @param string $dsid
 *   The DataStream ID.
 * @param string $delay
 *   The Delay, in seconds, between logging.
 * @param string $pid
 *   The PID of the modified object.
 */
function islandora_workflow_rest_set_ds_log($type, $user, $dsid, $delay, $pid) {
  db_insert('islandora_workflow_rest_ds_logs')
      ->fields(
          array(
            'type' => $type,
            'usr' => $user,
            'dsid' => $dsid,
            'delay' => $delay,
            'pid' => $pid,
          )
      )
      ->execute();
}

/**
 * Remove a Datastream Log Entry.
 *
 * @param string $pid
 *   The Fedora Objects PID.
 * @param string $dsid
 *   The DataStream ID.
 * @param string $user
 *   The user making the modification.
 */
function islandora_workflow_rest_remove_ds_log($pid, $dsid = "", $user = "") {
  db_delete('islandora_workflow_rest_ds_logs')
      ->condition('pid', $pid)
      ->condition('dsid', $dsid)
      ->condition('usr', $user)
      ->execute();
}

/**
 * Return a list of Data Stream logs by PID.
 *
 * @param string $pid
 *   The pid of the object to be queried.
 *
 * @return array
 *   An array of Data Stream logs by PID.
 */
function islandora_workflow_rest_get_ds_logs_by_pid($pid) {
  $list = db_select('islandora_workflow_rest_ds_logs', 'c')
      ->fields('c')
      ->condition('pid', $pid)
      ->execute();
  return $list;
}

/**
 * Retrieve existing ds logs, via pager.
 *
 * @param int $limit
 *   The number to limit the page results to.
 *
 * @return array
 *   An array of Monitor log entry's.
 */
function islandora_workflow_rest_get_all_ds_logs($limit = 10) {
  $query = db_select('islandora_workflow_rest_ds_logs', 'r');
  $query->fields('r', array("lid", "type", "dsid", "delay", "pid", "usr"));
  $table_sort = $query->extend('TableSort');
  $pager = $table_sort->extend('PagerDefault')->limit($limit);
  $ds_logs = $pager->execute();

  return $ds_logs;
}

/**
 * Set a new ds log admin entry.
 *
 * @param string $dsid
 *   The DSID to update.
 * @param string $delay
 *   The new Delay, in seconds, between logging.
 * @param string $pid
 *   The PID of the Content Model to update.
 * @param string $label
 *   The label of the Content Model.
 */
function islandora_workflow_rest_set_ds_log_admin($dsid, $delay, $pid, $label) {
  db_insert('islandora_workflow_rest_ds_log_admin')
      ->fields(
          array(
            'dsid' => $dsid,
            'delay' => $delay,
            'pid' => $pid,
            'label' => $label,
          )
      )
      ->execute();
}

/**
 * Update an existing ds log admin entry.
 *
 * @param string $pid
 *   The PID of the Content Model to update.
 * @param string $dsid
 *   The DSID to update.
 * @param string $delay
 *   The new Delay, in seconds, between logging.
 */
function islandora_workflow_rest_update_ds_log_admin($pid, $dsid = "", $delay = "") {
  db_update('islandora_workflow_rest_ds_log_admin')
      ->fields(array('dsid' => $dsid, 'delay' => $delay))
      ->condition('pid', $pid)
      ->execute();
}

/**
 * Get ds admin log entry by pid.
 *
 * @param string $pid
 *   The PID to retrieve ds log entrys for.
 *
 * @return array
 *   An array of ds log entry's.
 */
function islandora_workflow_rest_get_ds_log_admin_by_pid($pid) {
  $list = db_select('islandora_workflow_rest_ds_log_admin', 'c')
      ->fields('c')
      ->condition('pid', $pid)
      ->execute();
  return $list;
}

/**
 * Delete ds admin log entry.
 *
 * @param string $pid
 *   The Content Model PID to remove entry.
 */
function islandora_workflow_rest_delete_ds_logs_admin($pid) {
  db_delete('islandora_workflow_rest_ds_log_admin')
      ->condition('pid', $pid)
      ->execute();
}

/**
 * Get all ds admin logs.
 *
 * @param int $limit
 *   Limit the results, used via pager.
 *
 * @return array
 *   An array of admin logs.
 */
function islandora_workflow_rest_get_all_ds_logs_admin($limit = 10) {
  $query = db_select('islandora_workflow_rest_ds_log_admin', 'r');
  $query->fields('r', array("lid", "pid", "label", "dsid", "delay"));
  $table_sort = $query->extend('TableSort');
  $pager = $table_sort->extend('PagerDefault')->limit($limit);
  $ds_logs = $pager->execute();
  return $ds_logs;
}

/**
 * Gets the payload from a PUT/DELETE request.
 *
 * HTTP PUT/DELETE isn't well supported in PHP, so this is our only option.
 *
 * @return array
 *   The JSON decoded payload if defined.
 */
function islandora_workflow_rest_get_request_body() {
  $body = &drupal_static(__FUNCTION__);
  if (isset($body)) {
    return $body;
  }
  if (isset($_SERVER['CONTENT_LENGTH']) && $_SERVER['CONTENT_LENGTH'] > 0) {
    $stdin = fopen("php://input", "r");
    $body = fread($stdin, $_SERVER['CONTENT_LENGTH']);
    $content_type = isset($_SERVER["CONTENT_TYPE"]) ? $_SERVER["CONTENT_TYPE"] : '';
    switch ($content_type) {
      case 'application/json':
      default:
        // At the moment we are assume only JSON will be included in the request
        // body. To support multi-part PUT request we would have to implement
        // some request body parser here.
        $body = drupal_json_decode($body);
        break;
    }
  }
  return $body;
}

/**
 * Get the request parameters.
 *
 * @return array
 *   An array containing the request parameters.
 */
function islandora_workflow_rest_get_request_parameters() {
  switch ($_SERVER['REQUEST_METHOD']) {
    case 'GET':
      return $_GET;

    case 'POST':
      // Can POST Fields or Content Body, prefer Fields.
      if (empty($_POST)) {
        return islandora_workflow_rest_get_request_body();
      }
      return $_POST;

    case 'PUT':
    case 'DELETE':
      return islandora_workflow_rest_get_request_body();

    default:
      throw new Exception('Method Not Allowed', 405);
  }
}

/**
 * Admin autocomplete callback which returns solr fields from Luke.
 *
 * @param string $string
 *   String filled out in the autocomplete textfield.
 *
 * @return json
 *   A json array containing the Solr luke values that contain the given string.
 */
function islandora_workflow_rest_autocomplete_luke($string = '') {
  module_load_include('inc', 'islandora_solr', 'includes/luke');
  $luke = islandora_solr_get_luke();
  $result = array();
  foreach ($luke['fields'] as $term => $value) {
    if (stripos($term, $string) !== FALSE) {
      // Search case insensitive, but keep the case on replace.
      $term_str = preg_replace("/$string/i", "<strong>\$0</strong>", $term);

      // Add strong elements to highlight the found string.
      $result[$term] = $term_str . '<strong style="position: absolute; right: 5px;">(' . $value['type'] . ')</strong>';
    }
  }
  // Sort alphabetically.
  ksort($result);

  drupal_json_output($result);
  exit();
}

/**
 * Pretty print datastream content.
 *
 * @param string $content
 *   An objects datastream content.
 *
 * @return array
 *   Formated xml/html for content.
 */
function islandora_workflow_rest_pretty_print_ds($content) {
  module_load_include('inc', 'php_lib', 'DOMHelpers');
  $doc = new DOMDocument();
  @$doc->loadXML($content);
  $return = @dom_document_pretty_print($doc, TRUE);
  return str_replace('&amp;#xD;', "", $return);
}

/**
 * Retrive an array of accounts for a give CSV of usernames.
 *
 * @param string $string
 *   CSV string with valid drupal user names.
 *
 * @return array
 *   An array of user accounts.
 */
function islandora_workflow_rest_load_users_from_csv($string, $use_realname = FALSE) {
  $accounts = array();
  $pieces = explode(",", $string);
  try {
    foreach ($pieces as $key => $value) {
      $accounts[] = $use_realname ? islandora_workflow_rest_load_by_real_name($value) : user_load_by_name($value);
    }
  }
  catch (Exception $e) {
    throw new Exception('Supplied account does not exist on this system');
  }
  return $accounts;
}

/**
 * Autocomplete callback for user's names.
 *
 * @param string $value
 *   Full or part of a users name.
 * @param bool $use_realname
 *   Look up names from the realnames table, if it exists.
 *
 * @throws Exception
 * @return array
 *   An array of possible matches.
 */
function islandora_workflow_rest_find_user_autocomplete($value, $use_realname = FALSE) {
  $return = array();
  try {
    if ($use_realname) {
      // Preform join on users and the realname table.
      $query = db_select('users', 'u');
      $query->join('realname', 'rn', 'u.uid = rn.uid');
      $query->fields('u');
      $query->fields('rn');
      $query->condition('rn.realname', db_like($value) . '%', 'LIKE');
      $query->groupBy('u.uid');
      $query->range(0, 100);
      $result = $query->execute();
      foreach ($result as $user) {
        $return[] = $user;
      }
      return $return;
    }
    // If realname module is not enabled, default to the users table.
    $result = db_select('users')
      ->fields('users', array('name'))
      ->condition('name', db_like($value) . '%', 'LIKE')
      ->range(0, 10)
      ->execute();
    return $result;
  }
  catch (Exception $e) {
    throw new Exception($e);
  }
  return FALSE;
}

/**
 * Loads a user account by real name value.
 *
 * @param string $value
 *   The real name value.
 *
 * @return array|boolean
 *   FALSE if an account is found, or array of accounts.
 */
function islandora_workflow_rest_load_by_real_name($value) {
  $result = db_select('realname')
    ->fields('realname', array('uid', 'realname'))
    ->condition('realname', $value, '=')
    ->execute();
  foreach ($result as $user) {
    $drup_user = user_load($user->uid, TRUE);
    return $drup_user;
  }
  return FALSE;
}

/**
 * Convert a Real Name CSV string to a drupal user CSV string.
 *
 * @param string $string
 *   The Real Name CSV string to transform
 *
 * @return string
 *   The drupal user name CSV string.
 */
function islandora_workflow_rest_transform_user_csv($string) {
  $return = $string;
  if (module_exists('realname')) {
    // We need the drupal user name, not the real name entry.
    $pieces = explode(",", $string);
    $accounts;
    $return = "";
    foreach ($pieces as $key => $value) {
      $user = islandora_workflow_rest_load_by_real_name($value);
      $return .= $user->name . ",";
    }
  }
  return rtrim($return, ",");
}
/**
 * Transform a workflow entry into email text.
 *
 * @param array $workflow_item
 *   The workflow item as array to transform.
 * @param string $switch
 *   'current' or 'previous', indicating step.
 *
 * @return string
 *   The string entry for notification emails.
 */
function islandora_workflow_rest_prepare_workflow_string($workflow_item, $switch) {
  $concat = ($switch == "current" ? "Current " : "Previous ");
  // Check if there IS a workflow entry.
  $text = "";
  if (isset($workflow_item['response'])) {
    $text = "$concat Workflow Entry (" . $workflow_item['response']['workflow']['attributes']['workflowID'] . "):\n";
    $text .= "--Stamp: " . $workflow_item['response']['workflow']['activity']['attributes']['category'] . "\n";
    $text .= "--Category: " . $workflow_item['response']['workflow']['activity']['attributes']['stamp'] . "\n";
    $text .= "--Status: " . $workflow_item['response']['workflow']['activity']['attributes']['status'] . "\n";
  }
  return $text;
}

/**
 * Email helper, using drupal_mail.
 *
 * @param array $params
 *   Required email notification parameters.
 */
function islandora_workflow_rest_notify($params) {
  foreach ($params['accounts'] as $account) {
    $params['mail_account'] = $account;
    drupal_mail(
        'islandora_workflow_rest', 'notice', $account->mail, user_preferred_language($account), $params, "donotreplay@site.com", TRUE
    );
  }
}

/**
 * Create the configure content models table.
 *
 * @return array
 *   The form as array to be rendered by drupal.
 */
function islandora_workflow_rest_admin_cm_table_form() {
  global $user;
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
  $form = array();
  $content_models = islandora_get_content_models();

  $data = islandora_workflow_rest_get_all_ds_logs_admin(1000);
  $options = array();
  $item_checked = array();
  // All stored content models.
  while ($content_model = $data->fetchObject()) {
    $monitor_datastream = array(
      '#type' => 'textfield',
      '#value' => $content_model->dsid,
      '#title' => 'Datastream ID',
      '#description' => 'Comma separated datastream values.',
      '#title_display' => 'invisible',
      '#name' => $content_model->pid . '[dsid_entry]',
    );
    $logging_delay = array(
      '#type' => 'textfield',
      '#value' => $content_model->delay,
      '#title' => 'Logging Delay',
      '#description' => 'Datastream logging delay in seconds.',
      '#title_display' => 'invisible',
      '#name' => $content_model->pid . '[delay_entry]',
    );
    $options[$content_model->pid]['pid'] = $content_model->pid;
    $options[$content_model->pid]['label'] = $content_model->label;
    $options[$content_model->pid]['dsid'] = array('data' => $monitor_datastream);
    $options[$content_model->pid]['delay'] = array('data' => $logging_delay);
    $item_checked[$content_model->pid] = 1;
  }
  $set_keys = array_keys($options);
  foreach ($options as $ckey => $value) {
    unset($content_models[$ckey]);
  }
  foreach ($content_models as $key => $value) {
    // Add any additional content models as available.
    $monitor_datastream = array(
      '#type' => 'textfield',
      '#value' => 'TN',
      '#title' => 'Datastream ID',
      '#description' => 'Comma separated datastream values.',
      '#title_display' => 'invisible',
      '#name' => $key . '[dsid_entry]',
    );
    $logging_delay = array(
      '#type' => 'textfield',
      '#value' => '30',
      '#title' => 'Logging Delay',
      '#description' => 'Datastream logging delay in seconds.',
      '#title_display' => 'invisible',
      '#name' => $key . '[delay_entry]',
    );
    $options[$key]['pid'] = $key;
    $options[$key]['label'] = $value['label'];
    $options[$key]['dsid'] = array('data' => $monitor_datastream);
    $options[$key]['delay'] = array('data' => $logging_delay);
    $item_checked[$key] = 0;
  }
  $header = array(
    'label' => t('Label'),
    'pid' => t('PID'),
    'dsid' => t('Datastream ID'),
    'delay' => t('Logging Delay'),
  );
  $form['content_model_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#default_value' => $item_checked,
    '#attributes' => '',
    '#empty' => t("There are no content models??"),
  );
  return $form;
}

/**
 * Log datastream modification event.
 *
 * @global user $user
 *
 * @param string $pid
 *   PID of object being modified
 * @param string $action
 *   CRUD identifier
 * @param string $dsid
 *   DSID of datastrream being modified
 * @param string $delay
 *   Event will not be logged if change has been logged within delay time
 */
function islandora_workflow_log_event($pid, $label, $action, $dsid, $delay, $timestamp, $last_stamp = NULL) {
  global $user;
  $threshold = strtotime($timestamp) - intval($delay);
  $user_id = $user->uid;
  $query = "SELECT * from {islandora_workflow_rest_ds_logs}
             where pid = '$pid'
             and type = '$action'
             and usr = '$user_id'
             and dsid = '$dsid'
             and state = 'new'
             and timestamp > $threshold";
  $result = db_query($query)->fetchAssoc();
  if ($result) {
    $lid = $result['lid'];
    db_update('islandora_workflow_rest_ds_logs')
        ->fields(array('timestamp' => strtotime($timestamp)))
        ->condition('lid', $lid)
        ->execute();
  }
  else {
    db_insert('islandora_workflow_rest_ds_logs')
        ->fields(array(
          'pid' => $pid,
          'object_label' => $label,
          'type' => $action,
          'usr' => $user->uid,
          'dsid' => $dsid,
          'delay' => $delay,
          'timestamp' => strtotime($timestamp),
          'state' => 'new',
          'last_stamp' => $last_stamp,
        ))
        ->execute();
  }
}

/**
 * Perform update function from dtastream add and alter hooks.
 *
 * @param AbstractObject $object
 *   Object representing Fedora parent object
 * @param AbstractDatastream $datastream
 *   Object representing fedora datastream
 * @param string $action
 *   CRUD action
 */
function islandora_workflow_add_or_modify(AbstractObject $object, AbstractDatastream $datastream, $action) {
  module_load_include('inc', 'islandora_workflow_rest', 'includes/cwrc_workflow');
  module_load_include('inc', 'islandora_workflow_rest', 'includes/utilities');
  $tracked = FALSE;
  $delay = 0;
  $tuque = new IslandoraTuque();
  $tuque->cache->delete($object->id);
  $object = islandora_object_load($object->id);
  $datastream = $object[$datastream->id];
  foreach ($object->models as $model) {
    $result = islandora_workflow_rest_get_ds_log_admin_by_pid($model);
    while ($record = $result->fetchAssoc()) {
      $tracked_dsids = array_map('trim', explode(',', $record['dsid']));
      if (in_array($datastream->id, $tracked_dsids)) {
        $delay = $record['delay'];
        $tracked = TRUE;
        $fedora_timestamp = $datastream->createdDate;
        break;
      }
    }
  }
  if ($tracked) {
    $last_workflow_step = NULL;
    if ($object['WORKFLOW']) {
      $workflow = CWRCWorkflowAPI::fromDatastream($object['WORKFLOW']);
      $last_workflow_step = $workflow->entryToArray();
    }
    $stamp_exists = isset($last_workflow_step['response']['workflow']['activity']['attributes']['stamp']);
    $last_stamp = $stamp_exists ? $last_workflow_step['response']['workflow']['activity']['attributes']['stamp'] : t('None');
    islandora_workflow_log_event($object->id, $object->label, $action, $datastream->id, $delay, $fedora_timestamp, $last_stamp);
  }
}
