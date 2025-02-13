Sanity test list to perform on several platforms prior to release. 

## Test Data


## App
anything?

## Map Toolbar
### Table of Contents
### Basemap Picker
### Add Local Data
### Message Feeds


## Map Display Tools
### Navigation Tools
### Location Text
### Context Menu
- Coordinates
- Elevation
- Follow
- Identify
- Line of Sight
- Observation (see Observation Report section below)
- Viewshed
### Identify Tool
### Display Current Location
### Follow Position
### Coordinate Conversion Tool


### Tool Errors



## Message Feeds
### Broadcast Current Location


## Observation Report
### Test case 1: Create Observation Report from Tool
- go to the reports category and select the "Observation" tool icon
  - [ ] the contact report wizard should appear
  - [ ] the username should be whatever your app is configured to use (by default the machine name)
- step through the wizard, providing some input text for each of the fields
- on the Location page, use the click location mode (looks like a pointing finger) to set the location of the report by clicking in the view
  - [ ] you should see a red pulsing graphic in the view at that pos and the coordinates should be entered in the UI
- step through the rest of the wizard till you get to the Create Report page
  - [ ] the details you entered should be summarized and the "Create" button at the bottom of the page should be enabled
- click Create
  - [ ] you should see the contact report added in both apps
  - [ ] in the other app, identify the report (using the context menu) and confirm the details you submitted have been correctly broadcast

### Test case 2: Create Observation Report from Context Menu
- long press on the view to open the Context Menu
  - [ ] you should see an option for Observation Report
- select the option
  - [ ] the Contact Report should open and you should see the location graphic indicating the osition you launched the context menu from
- step through the wizard. On the location page use the current position icon (looks like a GPS cross-hair) to update the position to the current app location
  - [ ] the location should update the next time the apps position is updated (you may need to pan to find it)
- run through the rest of the wizard. On the final page click Cancel
  - [ ] the report should be cleared and the location highlight graphic should disappear

## Analysis Tools
### Line of Sight
### Test case 1: Enable Line of Sight
- turn on the location display
- go to the analysis tab and select "Line of sight"
- select "Observers_SD" from the overlay drop down
  - [ ] line of sight should be displayed from the various observer positions (atop buildings). 
  - [ ] Should look something like this:
### Test case 2: Toggle visibility
- uncheck the box next to the overlay drop down
  - [ ] the line of sight should disappear
- recheck the box
 - [ ] they should reappear

### Test case 3: Remove
- click the "x" button next to the overlay drop down
  - [ ] the lines of sight should disappear and the overlay drop down should be cleared

### Viewshed
#### Test case 1: Current Location viewshed
- open the "Viewshed" tool from the analysis toolbar (click on the tool icon)
- select "Current Location"
  - [ ] the follow position HUD should turn on if it was not enabled
  - [ ] a 360 degree viewshed should appear around the current location
- adjust the distance slider
  - [ ] the viewshed in the view should respond to the distance changes
- toggle 360 mode off
  - [ ] the viewshed should switch to a single viewshed segment which follows the heading of the current location
- adjust Horizontal Angle and Vertical Angle from the angle selector
  - [ ] for each angle, the viewshed in the scene should update accordingly
- click cancel
  - [ ] the viewshed should be removed
- create another viewshed around the current location but this time click Save
  - [ ] the viewshed should remain in the view

#### Test case 2: Map Element viewshed
- open the "Viewshed" tool from the analysis toolbar (click on the tool icon)
- select "Map Element"
- click on a graphic (e.g. from a message feed) in the view
  - [ ] a 360 degree viewshed should be created around the graphic and should follow it around the scene
- click on another graphic, the viewshed should switch to center on the new graphic
- adjust the distance slider
  - [ ] the viewshed should update to reflect the changed distance
- toggle off 360 mode
  - [ ] the viewshed should be replaced with a single segment
- experiment with changing the various angles (Heading, Pitch, Horizontal & Vertical angles)
  - [ ] the viewshed should update accordingly
- click Save
- the viewshed should remain in the scene

#### Test case 3: Follow Touch viewshed
- open the "Viewshed" tool from the analysis toolbar (click on the tool icon)
- select "Follow Touch"
- click on the terrain in the view
  - [ ] a 360 degree viewshed should be created around the point you clicked
- click on another location, the viewshed should move to the new position
- adjust the distance slider
  - [ ] the viewshed should update to reflect the changed distance
- toggle off 360 mode
  - [ ] the viewshed should be replaced with a single segment
- experiment with changing the various angles (Heading, Pitch, Horizontal & Vertical angles)
  - [ ] the viewshed should update accordingly
- click Save
- the viewshed should remain in the scene

#### Test case 4: Manage viewsheds
- click on View List
  - [ ] you should see each of the viewsheds created above
  - [ ] any 360 degree viewsheds should appear as a single list entry
- toggle the visibility of viewsheds in the list
  - [ ] the viewsheds should appear/disappear from the view
- open the sub menu for a viewshed and click Zoom To
  - [ ] the view should zoom to the viewshed
- open the sub menu and click Remove
  - [ ] the viewshed should be removed from the list and the view. 



## Alerts and Conditions


## Markup Tool

## App and Settings
### DsaConfig file
### Persist Layers
### Settings
### App Configuration
