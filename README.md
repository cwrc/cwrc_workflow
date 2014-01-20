
CONTENTS OF THIS FILE
---------------------

 * [Summary](#summary)
 * [Requirements](#requirements)
 * [Installation](#installation)
 * [Configuration](#configuration)
 * [Documentation](#documentation)
   * [Describe Existing Workflow Datastream](#describe-an-existing-workflow-datastream)
   * [Get Last Workflow Entry](#get-last-workflow-entry)
   * [Add Workflow Entry](#add-workflow-entry)
   * [Test object for a given stamp category or status](#test-object-for-a-given-stamp-category-or-status)
 * [Todo](#todo)

SUMMARY
-------
This module provides a number of REST end points for fetching/manipulating
workflow items.

REQUIREMENTS
------------

  * [Islandora](http://github.com/islandora/islandora)

INSTALLATION
------------

Assumes the following global XACML polices have been removed from:

$FEDORA_HOME/data/fedora-xacml-policies/repository-policies/default

* deny-inactive-or-deleted-objects-or-datastreams-if-not-administrator.xml
* deny-policy-management-if-not-administrator.xml
* deny-purge-datastream-if-active-or-inactive.xml
* deny-purge-object-if-active-or-inactive.xml

This module will still function with those policies in place but the tests this
module defines will fail. Also when installing Islandora don't forget to deploy
the policies it includes!


CONFIGURATION
-------------

For each of the REST end-points defined below in the documentation section there
exists a corresponding Drupal Endpoint. Making __GET__ requests to these endpoints
produces a json object representative of the requested task.


### Documentation Key
{variable} Required Parameter.

[variable] Optional Parameter no default defined, NULL or empty string likely be
used.

[variable, ’default’] Optional Parameter and it’s default.

### Common Responses
Unless otherwise specified each end-point can return the following responses.

#### Response: 401 Unauthorized
##### No response body.
In general 401 means an anonymous user attempted some action and was denied.
Either the action is not granted to anonymous uses in the Drupal permissions,
or XACML denied the action for the anonymous user.

#### Response: 403 Forbidden
##### No response body.
In general 403 means an authenticated user attempted some action and was denied.
Either the authenticated user does not have a Drupal role that grants him/her
permission to perform that action, or XACML denied the action for that user.

#### Response: 404 Not Found
##### No response body.
In general a 404 will occur, when a user tries to perform an action on a
object or datastream that doesn't not exist. Or XACML is hiding that
object or datastream from the user.

404 Responses can be returned even if the user was **not** determined to have
permission to perform the requested action, as the resource must be first
fetched from fedora before the users permission can be determined.

#### Response: 500 Internal Server Error
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| message       | A detail description of the error.

Any problem can trigger a 500 error, but in general you'll find that this is
typically an error returned by Fedora.

## Describe Existing Workflow Datastream

#### URL syntax
islandora_workflow_rest/v1/get_full_workflow/[params}

#### HTTP Method
GET

#### Headers
Accept: application/json

#### Get Parameters
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| PID           | Persistent identifier of the object

#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| workflow           | A Workflow array as json, with each entry representing a workflow item.

#### Example Response
```JSON
{
   "cwrc":{
      "#text":[],
      "workflow":[
         {
            "attributes":{
               "date":"2014-01-16",
               "userID":"1",
               "time":"18:20:54",
               "workflowID":"islandora_9_wk_0"
            },
            "activity":{
               "attributes":{
                  "category":"sample_category",
                  "stamp":"niso:AO",
                  "status":"sample_status"
               },
               "note":"This is the body of the note element"
            },
            "assigned":{
               "attributes":{
                  "category":"content_contribution"
               },
               "message":{
                  "recipient":{
                     "attributes":{
                        "userID":"Me"
                     }
                  },
                  "subject":"Test Subject",
                  "body":"This is a test entry. Additonal text required."
               }
            }
         }
      ]
   }
}
```

## Get Last Workflow Entry

#### URL syntax
islandora_workflow_rest/v1/get_last_workflow/{params}

#### HTTP Method
GET

#### Headers
Accept: application/json

#### GET
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pid           | The fedora identifier to find the last workflow enry for.

#### Response: 201 Created
##### Content-Type: application/json
Returns the same response as a [GET Object](#response-200-ok) request.

#### Example Response
```JSON
{
   "response":{
      "workflow":{
         "attributes":{
            "date":"2014-01-16",
            "userID":"1",
            "time":"19:54:48",
            "workflowID":"islandora_9_wk_3"
         },
         "activity":{
            "attributes":{
               "category":"sample remote",
               "stamp":"niso:AO",
               "status":"foo_bar"
            },
            "note":"sample text body"
         },
         "assigned":{
            "attributes":{
               "category":"content_contribution"
            },
            "message":{
               "recipient":{
                  "attributes":{
                     "userID":"herbie"
                  }
               },
               "subject":"muhaha",
               "body":"sample remote text for body"
            }
         }
      }
   }
}
```

## Add Workflow Entry

#### URL syntax
islandora_workflow_rest/v1/add_workflow/{params}

#### HTTP Method
GET

#### Headers
Accept: application/json

Content-Type: application/json

#### Get Variables
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| PID           | Persistent identifier of the object, if not given the namespace will be used to create a new one (required).
| toolID           | The tool identifier to add to the workflow (optional).
| activity           | Json object in key value pairs, E.X. {"category":"content_contribution","stamp":"nis:AO","status":"foo_bar","note":"Note text"} (optional).
| assigned           | Json object in key value pairs, E.X. {"category":"content_contribution","recipient":"user","subject":"foo_bar","body":"The body text"} (optional).


#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| response      | Json workflow item.

#### Example Response
```JSON
{
   "response":{
      "workflow":{
         "attributes":{
            "date":"2014-01-16",
            "userID":"1",
            "time":"19:54:48",
            "workflowID":"islandora_9_wk_3"
         },
         "activity":{
            "attributes":{
               "category":"sample remote",
               "stamp":"niso:AO",
               "status":"foo_bar"
            },
            "note":"sample text body"
         },
         "assigned":{
            "attributes":{
               "category":"content_contribution"
            },
            "message":{
               "recipient":{
                  "attributes":{
                     "userID":"herbie"
                  }
               },
               "subject":"muhaha",
               "body":"sample remote text for body"
            }
         }
      }
   }
}
```

## Test object for a given stamp category or status

#### URL syntax
islandora_workflow_rest/v1/has_entry/{params}

#### HTTP Method
GET

##### Get Variables
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pid           | Persistent identifier of the object.
| stamp           | (optional) The stamp to check for.
| category           | (optional) The category to check for.
| status           | (optional) The status to check for.
| simple           | (optional) Full workflow entries or boolean check. Default false.

#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| response      | Json workflow item.

#### Example Response (simple:false)
```JSON
{
   "response":{
      "workflow":{
         "attributes":{
            "date":"2014-01-16",
            "userID":"1",
            "time":"19:54:48",
            "workflowID":"islandora_9_wk_3"
         },
         "activity":{
            "attributes":{
               "category":"sample remote",
               "stamp":"niso:AO",
               "status":"foo_bar"
            },
            "note":"sample text body"
         },
         "assigned":{
            "attributes":{
               "category":"content_contribution"
            },
            "message":{
               "recipient":{
                  "attributes":{
                     "userID":"herbie"
                  }
               },
               "subject":"muhaha",
               "body":"sample remote text for body"
            }
         }
      }
   }
}
```

#### Example Response (simple:true)
```JSON
{
   "response":{
      "pid":{
         true
      }
   }
}
```

## List all PIDs with a given stamp/category and status within an optional collection

#### URL syntax
islandora_workflow_rest/v1/workflow_query/{params}

#### HTTP Method
GET

#### Headers
Accept: application/json

#### Get Variables
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pid           | Persistent identifier of the object.
| collection_id     | Limit workflow query to a collection. (optional)
| required     | Limit workflow query, required attribute fields. (optional)

#### Response: 200 OK
##### Content-Type: application/json
A JSON **array** with each field containing the following values.

| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pids     |  JSON Object array of pids matching the supplied query.

#### Example Response

```JSON
[{
  "pids": {
    "0": "islandora:1",
    "2": "islandora:2"
  }
}]
```

TODO
----
- [ ] Add input form for workflow.
- [ ] Implement workflow reports.
- [ ] Add table to view/edit workflow steps.
