<?php
/**
 * @file
 * Defines the CWRC_WORKFLOW class.
 */

class CWRCWorkflowAPI {

  /**
   * The XML/CWRC_WORKFLOW file/datastream.
   * @var DOMDocument
   */
  protected $doc;

  /**
   * The XPath for querying the CWRC_WORKFLOW XML file.
   * @var DOMXPath
   */
  protected $xpath;

  /**
   * The string location of the schema.
   * @var string
   */
  protected $schema;

  /**
   * Islandora object pid.
   * @var string
   */
  protected $pid;

  /**
   * Create a genric datastream from template.
   *
   * @return CWRCWorkflowAPI
   *   Instance of CWRCWorkflowAPI from plain template.
   */
  public static function fromTemplate() {
    $xml = <<<XML
<?xml version="1.0" encoding="UTF-8" ?>
<?xml-model href="http://cwrc.ca/schemas/workflow.rng" type="application/xml" schematypens="http://relaxng.org/ns/structure/1.0"?>
<cwrc></cwrc>
XML;
    $set_doc = new DOMDocument();
    @$set_doc->loadXML($xml);
    return new CWRCWorkflowAPI($set_doc);
  }

  /**
   * Create a CWRCWorkflowAPI from a datastream.
   *
   * @param AbstractDatastream $datastream
   *   A fedora_objects datastream.
   *
   * @return CWRCWorkflowAPI
   *   Returns CWRCWorkflowAPI object from datastream.
   */
  public static function fromDatastream(AbstractDatastream $datastream) {
    $set_doc = new DOMDocument();
    @$set_doc->loadXML($datastream->content);
    return new CWRCWorkflowAPI($set_doc);
  }

  /**
   * Solr query static function to find workflow pids, via query.
   *
   * @param string $custom_query
   *   Custom solr query.
   * @param string $collection_pid
   *   Collection pid to limit the results.
   * @param array $required
   *   An array of required attributes
   * @param int $offset
   *   Query offset.
   * @param int $limit
   *   Query limit.
   *
   * @return array
   *   An array of workflow items.
   */
  public static function findWorkflowPids($custom_query = NULL, $collection_pid = NULL, $required = array(), $offset = 0, $limit = 5, $simple_override = FALSE) {
    // Find all objects with a workflow datastream/attributes
    // in an optional collection.
    $query = ISLANDORA_WORKFLOW_REST_SOLR_DEFAULT_FIELD . ":*";
    $simple = TRUE;
    if (isset($custom_query)) {
      $query = $custom_query;
      $simple = FALSE;
    }
    if (isset($collection_pid)) {
      $query .= ' AND RELS_EXT_isMemberOfCollection_uri_mt:' . '"' . $collection_pid . '"';
      $simple = FALSE;
    }
    if (isset($required)) {
      foreach ($required as $key => $value) {
        $query .= "AND $key:" . '"' . $value . '"';
      }
    }
    $url = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));
    $solr = new Apache_Solr_Service($url['host'], $url['port'], $url['path'] . '/');
    $solr->setCreateDocuments(FALSE);
    try {
      $results = $solr->search($query, $offset, $limit);
      $data = json_decode($results->getRawResponse(), TRUE);
      if ($simple == TRUE || $simple_override == TRUE) {
        // We only want the PID's for these objects.
        foreach ($data['response']['docs'] as $key => $value) {
          $data[$key] = $data['response']['docs'][$key]['PID'];
        }
        unset($data['response']);
        unset($data['responseHeader']);
      }
      $json = $data;
    }
    catch (Exception $e) {
      watchdog_exception('Workflow item query: ', $e, 'Got an exception while searching for workflows.', array(), WATCHDOG_ERROR);
    }
    return $json;
  }

  /**
   * Updates an objects WORKFLOW datastream with the supplied CWRCWorkflowAPI.
   *
   * @param CWRCWorkflowAPI $work
   *   CWRCWorkflowAPI object representing a workflow node.
   * @param AbstractObject $object
   *   A fedora_object whos datastream 'WORKFLOW' should be updated.
   */
  public static function updateDatastream(CWRCWorkflowAPI $work, AbstractObject $object) {
    try {
      // Construct the datastream if it does not exist.
      if (!$object["WORKFLOW"]) {
        $ds = $object->constructDatastream("WORKFLOW");
        $ds->label = 'WORKFLOW DS';
        $ds->mimetype = 'text/xml';
        $ds->content = $work->toString();
        @$object->ingestDatastream($ds);
      }
      else {
        @$object['WORKFLOW']->content = $work->toString();
      }
    }
    catch (Exception $e) {
      throw new InvalidArgumentException($e);
    }
  }

  /**
   * Returns DOMDocument as xml string.
   */
  public function toString() {
    return $this->doc->saveXML();
  }

  /**
   * Retrieve a workflow step.
   *
   * Get a workflow step from this objects WORKFLOW
   * datastream. Defaults to the last entry.
   *
   * @param string $workflow_id
   *   (Optional) Return a specific workflow entry.
   */
  public function entryToArray($workflow_id = NULL) {
    $node = NULL;
    if ($workflow_id != NULL) {
      $node = $this->xpath->query("//*[@workflowID='" . $workflow_id . "']")->item(0);
    }
    else {
      $node = $this->getLastWorkflowStep();
    }
    if ($node == NULL) {
      return array();
    }
    // Create a new document.
    $newdoc = new DOMDocument();
    $newdoc->formatOutput = TRUE;
    $newdoc->loadXML("<response></response>");
    $node = $newdoc->importNode($node, TRUE);
    $newdoc->documentElement->appendChild($node);
    $xml_array = $this->xmlToArray($newdoc);
    return $xml_array;
  }

  /**
   * Return this objects DOMDocument as json object.
   *
   * @return array
   *   Current DOMDocument as json encoded array.
   */
  public function workflowToJson() {
    $xml_array = $this->xmlToArray($this->doc);
    return drupal_json_encode($xml_array);
  }

  /**
   * Convert an DOMDocument to an array().
   *
   * @param DOMDocument $root
   *   The DOMDocument to convert to formatted array.
   *
   * @return array
   *   Formatted DOMDocument array.
   */
  protected function xmlToArray($root) {
    $result = array();
    if ($root->hasAttributes()) {
      $attrs = $root->attributes;
      foreach ($attrs as $attr) {
        $result['attributes'][$attr->name] = $attr->value;
      }
    }
    if ($root->hasChildNodes()) {
      $children = $root->childNodes;
      if ($children->length == 1) {
        $child = $children->item(0);
        if ($child->nodeType == XML_TEXT_NODE) {
          $result['_value'] = $child->nodeValue;
          return count($result) == 1 ? $result['_value'] : $result;
        }
      }
      $groups = array();
      foreach ($children as $child) {
        if (!isset($result[$child->nodeName])) {
          $result[$child->nodeName] = $this->xmlToArray($child);
        }
        else {
          if (!isset($groups[$child->nodeName])) {
            $result[$child->nodeName] = array($result[$child->nodeName]);
            $groups[$child->nodeName] = 1;
          }
          $result[$child->nodeName][] = $this->xmlToArray($child);
        }
      }
    }
    return $result;
  }

  /**
   * Returns a clone of this object's DOMDocument.
   *
   * @return DOMDocument
   *   This objects DOMDocument.
   */
  public function toDOMDocument() {
    return clone $this->doc;
  }

  /**
   * Instantiate an CWRCWorkflow object.
   *
   * @param DOMDocument $dom_doc
   *   The cwrc_workflow datastream as DOMDocument
   *
   * @throws InvalidArgumentException
   */
  public function __construct(DOMDocument $dom_doc) {
    // TODO: Would like to move this out..
    $module_path = drupal_get_path('module', 'islandora_workflow_rest');

    $this->doc = $dom_doc;
    $this->schema = "http://cwrc.ca/schema/workflow.rng";
    // Setup xpath for querying on this object.
    $this->xpath = new DOMXPath($this->doc);
    $this->xpath->registerNamespace("php", "http://php.net/xpath");
    $this->xpath->registerPhpFunctions();
  }

  /**
   * Validate the dom schema against the DOM.
   * @throws InvalidArgumentException
   */
  protected function validateSchema() {
    try {
      $valid = $this->doc->relaxNGValidate($this->schema);
      if (!$valid) {
        drupal_set_message(t("This WORKFLOW datastream is not valid vs @schema", array('@schema' => $this->schema)), 'error');
      }
    }
    catch (Exception $e) {
      throw new InvalidArgumentException($e);
    }
  }

  /**
   * Create default attributes 'date', 'userID', 'time' and 'workflowID'.
   *
   * @param WorkflowItem $workflow
   *   WorkflowItem object representing the workflow node.
   */
  protected function createDefaultParamsForWorkflow(WorkflowItem $workflow) {
    $date = new DateTime();
    $result = $date->format('Y-m-d');
    $time = date('H:i:s');
    $user = user_uid_optional_load();
    $server_identifier = variable_get('islandora_workflow_rest_server_identifier', '');
    $workflow->typeAttributes[WorkflowConst::DATE] = $result;
    $workflow->typeAttributes[WorkflowConst::USERID] = $user->name . $server_identifier;
    $workflow->typeAttributes[WorkflowConst::TIME] = $time;
    $workflow->typeAttributes[WorkflowConst::WORKFLOWID] = $this->getNewWorkflowId();
  }

  /**
   * Generate a new workflowID.
   *
   * @return string
   *   Generated workflow id.
   */
  protected function getNewWorkflowId() {
    $workflow_elements = $this->doc->getElementsByTagName('workflow');
    $new_id = str_replace(":", "_", $this->pid);
    $workflow_count = 0;
    if (isset($workflow_elements)) {
      $workflow_count = $workflow_elements->length;
    }
    return $new_id .= "_wk_" . $workflow_count;
  }

  /**
   * Add a new workflow to the DOMDocument.
   *
   * @param unknown $pid
   *   islandora_objects pid, used to generate workflowId.
   * @param WorkflowItem $workflow
   *   WorkflowItem object representing the workflow node.
   * @param WorkflowItem $activity
   *   WorkflowItem object representing the activity node.
   * @param WorkflowItem $assigned
   *   WorkflowItem object representing the assigned node.
   */
  public function addWorkflowStep($pid, WorkflowItem $workflow, WorkflowItem $activity, WorkflowItem $assigned = NULL) {
    $this->pid = $pid;
    $parent = $this->doc->documentElement;
    $element = $this->doc->createElement($workflow->workName);
    $this->createDefaultParamsForWorkflow($workflow);
    foreach ($workflow->typeAttributes as $key => $value) {
      if (isset($value) && $value != "") {
        $element->setAttribute($key, $value);
      }
    }
    $parent->appendChild($element);
    if (isset($activity) && count($activity->typeAttributes) > 0) {
      $this->createActivity($element, $activity);
    }
    if (isset($assigned) && count($assigned->typeAttributes) > 0) {
      $this->createAssigned($element, $assigned);
    }
  }

  /**
   * Create an activity element for the workflow node.
   *
   * @param DOMElement $parent
   *   The parent element to add the activity to.
   * @param WorkflowItem $child
   *   The WorkflowItem to add to the parent.
   */
  protected function createActivity(DOMElement $parent, WorkflowItem $child) {
    $node = $this->doc->createElement($child->workName);
    foreach ($child->typeAttributes as $key => $value) {
      // Crazy if sidesteps the message for the assignment.
      if ($key == WorkflowConst::CATEGORY || $key == WorkflowConst::STAMP || $key == WorkflowConst::STATUS) {
        $node->setAttribute($key, $value);
      }
    }
    $parent->appendChild($node);
    // Add the note text if it is supplied.
    if (isset($child->typeAttributes[WorkflowConst::NOTE])) {
      $note = $this->doc->createElement(WorkflowConst::NOTE);
      $note_body = $this->doc->createTextNode($child->typeAttributes[WorkflowConst::NOTE]);
      $note->innerHTML = $child->typeAttributes[WorkflowConst::NOTE];
      $note->appendChild($note_body);
      $node->appendChild($note);
    }
    $message = $this->createMessage($child);
    if (!is_null($message)) {
      $node->appendChild($message);
    }
  }

  /**
   * Create an assigned element for the workflow node.
   *
   * @param DOMElement $parent
   *   The parent element to add the activity to.
   * @param WorkflowItem $child
   *   The WorkflowItem to add to the parent.
   */
  protected function createAssigned(DOMElement $parent, WorkflowItem $child) {
    $assigned = $this->doc->createElement($child->workName);
    $assigned->setAttribute("category", $child->typeAttributes['category']);

    if (isset($child->typeAttributes[WorkflowConst::NOTE])) {
      $note = $this->doc->createElement(WorkflowConst::NOTE);
      $note_body = $this->doc->createTextNode($child->typeAttributes[WorkflowConst::NOTE]);
      $note->innerHTML = $child->typeAttributes[WorkflowConst::NOTE];
      $note->appendChild($note_body);
      $assigned->appendChild($note);
    }

    $parent->appendChild($assigned);
    $message = $this->createMessage($child);
    if (!is_null($message)) {
      $assigned->appendChild($message);
    }
  }

  /**
   * Generate a message for assigned or activity nodes.
   *
   * @param WorkflowItem $child
   *   WorkflowItem with expected params.
   *
   * @return DOMElement
   *   The Dom Element as message.
   */
  protected function createMessage(WorkflowItem $child) {
    $message = NULL;
    if (isset($child->typeAttributes[WorkflowConst::RECIPIENT]) &&
        isset($child->typeAttributes[WorkflowConst::SUBJECT]) &&
        isset($child->typeAttributes[WorkflowConst::BODY])) {
      $message = $this->doc->createElement(WorkflowConst::MESSAGE);
      $recipient = $this->doc->createElement(WorkflowConst::RECIPIENT);
      $recipient->setAttribute("userID", $child->typeAttributes[WorkflowConst::RECIPIENT]);
      $message->appendChild($recipient);

      $subject = $this->doc->createElement(WorkflowConst::SUBJECT);
      $subject_note = $this->doc->createTextNode($child->typeAttributes[WorkflowConst::SUBJECT]);
      $subject->innerHTML = $child->text;
      $subject->appendChild($subject_note);
      $message->appendChild($subject);

      $body = $this->doc->createElement(WorkflowConst::BODY);
      $body_note = $this->doc->createTextNode($child->typeAttributes[WorkflowConst::BODY]);
      $body->innerHTML = $child->typeAttributes[WorkflowConst::BODY];
      $body->appendChild($body_note);
      $message->appendChild($body);
    }
    return $message;
  }

  /**
   * Get the last workflow step add to the datastream.
   */
  public function getLastWorkflowStep() {
    $elements = $this->doc->getElementsByTagName('workflow');
    $workflow_count = $this->doc->getElementsByTagName('workflow')->length;
    // TODO: safer to use the latest timestap then last element.
    return $elements->item($workflow_count - 1);
  }

  /**
   * Return a DOMNodeList as an array.
   *
   * @param DOMNodeList $domnodelist
   *   The DOMNodeList to convert to an array().
   *
   * @return array
   *   DOMNodeList converted to an array.
   */
  protected function dnl2array($domnodelist) {
    $return = array();
    for ($i = 0; $i < $domnodelist->length; ++$i) {
      $work_id = $domnodelist->item($i)->parentNode->getAttribute('workflowID');
      $return[] = $this->entryToArray($work_id);
    }
    return $return;
  }

  /**
   * Get a workflow entry by workflow id.
   *
   * @param string $workflow_id
   *   The workflow id to locate.
   */
  public function getWorkflowStepByWorkflowId($workflow_id) {
    return $this->getWorkflowStepsByAttributeValue(WorkflowConst::WORKFLOWID, $workflow_id);
  }

  /**
   * Query for a workflow element with a paticular attribute with value.
   *
   * @param string $attribute
   *   The attribute name to query for.
   * @param string $value
   *   The the attribute value.
   *
   * @return DOMElement
   *   The workflow DOMElement.
   */
  public function getWorkflowStepsByAttributeValue($attribute, $value = "") {
    $query = "//*[@" . $attribute . "='" . $value . "']";
    $tbody = $this->doc->getElementsByTagName('cwrc')->item(0);
    $workflow_steps = $this->xpath->query($query, $tbody);
    return $this->dnl2array($workflow_steps);
  }
}
