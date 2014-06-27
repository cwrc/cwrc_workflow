#CWRC Workflow

BUILD STATUS
------------
Current build status:
[![Build Status](https://travis-ci.org/discoverygarden/cwrc_workflow.png?branch=7.x)](https://travis-ci.org/discoverygarden/cwrc_workflow)

CI Server:
http://jenkins.discoverygarden.ca

CONTENTS OF THIS FILE
---------------------

 * [Summary](#summary)
 * [Requirements](#requirements)
 * [Installation](#installation)
 * [Configuration](#configuration)
 * [Documentation](#documentation)
   * [Describe Existing Workflow Datastream](#describe-existing-workflow-datastream)
   * [Get Last Workflow Entry](#get-last-workflow-entry)
   * [Add Workflow Entry](#add-workflow-entry)
   * [Test object for a given stamp category or status](#test-object-for-a-given-stamp-category-or-status)
   * [List all PIDs with a given stamp/category and status within an optional collection] (#list-all-pids-with-a-given-stampcategory-and-status-within-an-optional-collection)
 * [Todo](#todo)

SUMMARY
-------
This module provides a number of REST end points for fetching/manipulating
workflow items.


## Maintainers/Sponsors
-----------------------
Current maintainers:

* [CWRC](https://github.com/cwrc)

Past maintainers:

* [discoverygarden](https://github.com/discoverygarden)

This project has been sponsored by:

* [The Canadian Writing Research Collaboratory](http://www.cwrc.ca/en/)
The Canadian Writing Research Collaboratory is an online project designed to
enable unprecedented avenues for studying the words that most move people in and
about Canada.


## License
----------

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)



REQUIREMENTS
------------

  * [Islandora](http://github.com/islandora/islandora)

COMPLIMENTARY MODULES (RECOMMENDED)
-----------------------------------

  * [Real Name](https://drupal.org/project/realname)
  * [Privatemsg](https://drupal.org/project/privatemsg)

CONFIGURE COMPLIMTARY MODULES (OPTIONAL)
-----------------------------

### Privatemsg
By Default, the previous/last workflow entry is not included in email
notifications. After configuring the Real Name module as you like, you
can add these entry's to the notification email by navigating to
{your_site}/admin/config/messaging/privatemsg navigate to the email notify
tab, and add the following (including brackets) to the "Body of notification messages"
field:

[privatemsg_message:body]

### Real Name
Configure the Real Name module by preforming the following steps

* Navigate to {your_site}/admin/config/people/accounts/fields
* Add a new field (ex: field_full_name) as Field Type 'Text', click 'Save'
* Navigate to {your_site}/admin/config/people/realname, click browse available tokens
* Select the new field you created, and click 'Save configuration'



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
produces a JSON object representative of the requested task.


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
islandora_workflow_rest/v1/get_full_workflow/{params}

#### HTTP Method
GET

#### Headers
Accept: application/json

#### Get Parameters
| Name          | Description                            | Optional            |
| ------------- | -------------------------------------- | ------------------- |
| PID           | Persistent identifier of the object    | Required

#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| workflow      | A Workflow array, with each entry representing a workflow item.

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
| Name          | Description                                                | Optional    |
| ------------- | ---------------------------------------------------------- | ------------|
| PID           | The fedora identifier to find the last workflow entry for. | Required

#### Response: 200 OK
##### Content-Type: application/json

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
| Name          | Description                                         | Optional  |
| ------------- | --------------------------------------------------- | --------- |
| PID           | Persistent identifier of the object.                | Required
| toolID        | The tool identifier to add to the workflow          | Optional
| activity      | JSON with keys _category_, _stamp_, _status_        | Required
| assigned      | JSON with keys _category_                           | Optional

Both *activity* and *assigned*, have the optional keys _note_, _recipient_,
_subject_, and _body_. A notification email will only be sent if _recipient_,
_subject_, and _body_ are all set.

### *activity* Example
```JSON
{
    "category":"content_contribution",
    "stamp":"nis:AO",
    "status":"foo_bar",
    "note":"Note text",
}
```

### *assigned* Example
```JSON
{
    "category":"content_contribution",
    "recipient":"user",
    "subject":"foo_bar",
    "body":"The body text",
}
```

#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| response      | JSON workflow item.

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
| Name          | Description                            | Optional    |
| ------------- | -------------------------------------- | ----------- |
| PID           | Persistent identifier of the object.   | Required
| stamp         | The stamp to check for.                | Optional
| category      | The category to check for.             | Optional
| status        | The status to check for.               | Optional
| simple        | Full workflow entries or simple check. | Optional (Default: false)

#### Response: 200 OK
##### Content-Type: application/json
| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| response      | JSON object array of workflow item(s).

#### Example Response (simple:false)
```JSON
{
   "response":{
      "stamp":[
         {
            "response":{
               "workflow":{
                  "attributes":{
                     "date":"2014-03-04",
                     "userID":"1",
                     "time":"13:29:34",
                     "workflowID":"islandora_root_wk_0"
                  },
                  "activity":{
                     "attributes":{
                        "category":"CREATED",
                        "stamp":"islandora:AO",
                        "status":"In Progress"
                     },
                     "note":"dsafsafa",
                     "message":{
                        "recipient":{
                           "attributes":{
                              "userID":"usera,userb"
                           }
                        },
                        "subject":"mememe",
                        "body":"go away."
                     }
                  }
               }
            }
         },
         {
            "response":{
               "workflow":{
                  "attributes":{
                     "date":"2014-03-04",
                     "userID":"1",
                     "time":"13:32:50",
                     "workflowID":"islandora_root_wk_1"
                  },
                  "activity":{
                     "attributes":{
                        "category":"CREATED",
                        "stamp":"islandora:AO",
                        "status":"In Progress"
                     },
                     "note":"sadfasfasdf",
                     "message":{
                        "recipient":{
                           "attributes":{
                              "userID":"usera,userb"
                           }
                        },
                        "subject":"me",
                        "body":"mess"
                     }
                  }
               }
            }
         }
      ],
   }
}
```

#### Example Response (simple:true)
```JSON
{
  "response": {
    "stamp": "TRUE"
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
| Name           | Description                                                  | Optional    |
| -------------- | ------------------------------------------------------------ | ----------- |
| PID | Limit workflow query to a collection by collection PID.      | Optional
| required       | Limit workflow query VIA required attribute fields           | Optional
| query          | SOLR query string EX: 'PID:*'.                               | Optional

The *collection_pid* parameter and *required* parameter will be still be applied
to the *query* string if present.

### *required* Example
```JSON
{
    "workflow_workflowID_ms":"islandora_root_wk_1"
}
```

#### Response: 200 OK
##### Content-Type: application/json
A JSON **array** with each field containing the following values.

| Name          | Description                                                  |
| ------------- | ------------------------------------------------------------ |
| pids          |  JSON Object array of pids matching the supplied query.

#### Example Response

```JSON
[{
  "islandora:1",
  "islandora:2",
  "islandora:3"...
}]
```

TODO
----
- [ ] Add input form for workflow.
- [ ] Implement workflow reports.
- [ ] Add table to view/edit workflow steps.
