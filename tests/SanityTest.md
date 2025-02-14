Sanity test list to perform on several platforms prior to release. 

# 1. Setup / Test Data


# 2. Map Toolbar
## Table of Contents
#### Setup:
- Make sure you have some data to add to the app  in ~/ArcGIS/Runtime/Data/DSA/SimulationData. You will need some data in the same location - e.f. Shasta.tif and Shasta_elevation.tif rasters***

#### Steps:
- Run the DSA app
- Click on the Table of contents toolbar icon.
  - [ ] confirm that the Table of Contents opens and is empty
  - [ ] confirm that you can still interact with the map/scene (if in vehicle app)
- close the tool
- launch the add data tool and add some layers
- reopen the table of contents tool
  - [ ] confirm that the layers you added are in the list
  - [ ] confirm that each layer has an icon (points, lines, polygons, raster)
  - [ ] confirm that each layer has a name (there is a known issue about names for raster layers so we have a workaround for now)
- for each layer in turn: 
  - click on the item to zoom to the layer
    - [ ] confirm the map/scene zooms
  - toggle the layer visibility on/off
    - [ ] confirm the layer is set visible/invisible in the map/scene
- select the ellipses and opt to zoom to a layer
- experiment with reordering the layers by selecting the ellipses and selecting move up or down
  - [ ] confirm that reordering the list also reorders the layers in the map/scene
- remove a layer by selecting the ellipses
  - [ ] confirm the layer is removed from the list
  - [ ] confirm the layer is removed from the map/scene

## Basemap Picker
- Run the DSA app
- confirm that the app starts with the topographic basemap
- Select the basemap tool
- confirm that you see a list of all the .tpk files in the directory you copied to
- select one of the tpks
- confirm that the tool closes and the basemap changes
- reopen the tool
- click the close button
- confirm that the tool closes without changing the basemap

## Add Local Data
Setup:
- Copy some raster data (\\apps-data\data\api\qt\UnitTests\raster\formats\tiff\Monterey), a shapefile (\\apps-data\data\api\qt\UnitTests\shapefile), a geodatabase (\\apps-data\data\api\qt\UnitTests\Geodatabase), and a TPK (\\apps-data\data\api\qt\UnitTests\tpks) to ~/ArcGIS/Runtime/Data/DSA/OperationalData
- Copy a tpk that supports elevation (\\apps-data\data\api\qt\UnitTests\tpks\elevation\California\CaDem.tpk) to ~/ArcGIS/Runtime/Data/DSA/ElevationData

Steps:
- Run the DSA app
- Select Map Tools icon, then the Add Data tool 
- Change the filter to 'Raster Files' and select a raster; check the box for one of the raster layers. Click ADD SELECTED, this should add it to the map. To zoom to that layer, select the Overlays tool, click on, '...', and select 'Zoom to'.
- Select the Add Data tool again
- Change the filter to 'Geodatabase'
- Select a Geodatabase; check the box for one of the raster layers. Click ADD SELECTED, this should add it to the map. 
- Select the  the Add Data tool again, and change the filter to Tile Package
- Select a TPK that supports elevation; check the box for one of the raster layers; select the 'Add as elevation source' checkbox. Then click ADD SELECTED, this should add it to the map. 
- Select the  Add Data tool again
- Uncheck the add as elevation checkbox, and change the filter to'Shapefile'
- Select a shapefile and click ADD SELECTED toadd it to the map, and make sure it draws


## Message Feeds
Run the DSA app
Run the Message Simulator app
Run the temporary ArcGISQtMessageSimulator (for GeoMessage)

The DSA app by default should be configured to handle certain message feeds.
They are:
1.  SA Events - (Cursor-on-Target)
2.  Friendly Tracks - Land (GeoMessage)
3.  Friendly Tracks - Air (GeoMessage)
4.  Contact Reports (GeoMessage)
5.  Situation Reports (GeoMessage)
6.  EOD Reports (GeoMessage)
7.  Sensor Observations (GeoMessage)

**Test 1: SA Events message feed**

**Steps**

-  Run the DSA app (Vehicle or Handheld)
-  Run the MessageSimulator app and browse to the `CoT_Reports.xml` simulation file in the DSA's `SimulationData` folder.
-  Run the simulation on port 45678
-  Specify a frequency value for the amount of messages to send
-  Choose to loop simulation
-  Hit the `Start` button to start the simulation

**Expectations**

`SA Events` message feed should begin drawing mil2525c symbols to the screen.

**Test 2: Toggle SA Evenets message feed**

**Steps**
-  While the DSA app and `CoT_Reports.xml` simulation running, select the `Message Feeds` panel and switch off the `SA Events` message feed.

**Expectations**

The `SA Events` graphics should no longer be drawn to the view.

**Test 3: Friendly Tracks - Land message feed**

**Steps**

-  Run the DSA app (Vehicle or Handheld)
-  Run the ArcGISQtMessageSimulator (GeoMessage) app and browse to the `GeoMessage_FriendlyTracksLand.xml` simulation file in the DSA's `SimulationData` folder.
-  Run the simulation on port 45678
-  Specify a frequency value for the amount of messages to send
-  Choose to loop simulation
-  Hit the `Start` button to start the simulation

**Expectations**

`Friendly Tracks - Land` message feed should begin drawing mil2525c friendly draped symbols to the screen.

**Test 4: Toggle Friendly Tracks - Land message feed**

**Steps**
-  While the DSA app and `GeoMessage_FriendlyTracksLand.xml` simulation running, select the `Message Feeds` panel and switch off the `Friendly Tracks - Land` message feed.

**Expectations**

The `Friendly Tracks - Land` graphics should no longer be drawn to the view.

**Test 5: Friendly Tracks - Air message feed**

**Steps**

-  Run the DSA app (Vehicle or Handheld)
-  Run the ArcGISQtMessageSimulator (GeoMessage) app and browse to the `GeoMessage_FriendlyTracksAir.xml` simulation file in the DSA's `SimulationData` folder.
-  Run the simulation on port 45678
-  Specify a frequency value for the amount of messages to send
-  Choose to loop simulation
-  Hit the `Start` button to start the simulation

**Expectations**

`Friendly Tracks - Air` message feed should begin drawing mil2525c friendly symbols to the screen in absolute surface placement mode.  The symbols are floating in the air at a fixed elevation.

**Test 6: Toggle Friendly Tracks - Air message feed**

**Steps**
-  While the DSA app and `GeoMessage_FriendlyTracksAir.xml` simulation running, select the `Message Feeds` panel and switch off the `Friendly Tracks - Air` message feed.

**Expectations**

The `Friendly Tracks - Air` graphics should no longer be drawn to the view.

**Test 7: Toggle Friendly Tracks labels**

**Steps**
- While the DSA app and `GeoMessage_FriendlyTracksLand.xml` simulation running, select the `Message Feeds` panel and make sure the `Friendly Tracks - Land` message feed is enabled.
- Confirm that labels are displayed next to the blue graphics.
- Open up the settings panel and uncheck the "Show friendly tracks labels" checkbox.
- Confirm that labels are not displayed anymore next to the blue graphics.
- Check the "Show friendly tracks labels" checkbox back on.
- Confirm that labels are displayed next to the blue graphics again.

**Test 8: Contact Reports, Situation Reports, EOD Reports, and Sensor Observations message feeds**

**Steps**

-  Run the DSA app (Vehicle or Handheld)
-  Run the ArcGISQtMessageSimulator (GeoMessage) app and browse to the `GeoMessage_MontereyReports.xml` simulation file in the DSA's `SimulationData` folder.
-  Run the simulation on port 45678
-  Specify a frequency value for the amount of messages to send
-  Choose to loop simulation
-  Hit the `Start` button to start the simulation

**Expectations**

`Contact Reports`, `Situation Reports`, `EOD Reports`, and `Sensor Observations` message feeds should begin drawing picture marker symbol types to the screen for each feed.

**Test 9: Toggle Contact Reports, Situation Reports, EOD Reports, and Sensor Observations message feeds**

**Steps**
-  While the DSA app and `GeoMessage_MontereyReports.xml` simulation running, select the `Message Feeds` panel and switch off the `Contact Reports`, `Situation Reports`, `EOD Reports`, and `Sensor Observations` message feeds.

**Expectations**

The `Contact Reports`, `Situation Reports`, `EOD Reports`, and `Sensor Observations` message feeds graphics should no longer be drawn to the view.


# 3. Map Display Tools
## Navigation Tools
Run the DSA app

**Test 1: Zoom in**

**Steps:**
1. Click on the `ZoomIn` tool from the Navigation toolbar 
![image](https://devtopia.esri.com/storage/user/163/files/634c4ee8-adcd-11e7-9264-df99e0d2c9a1).

**Expectations**
- [ ] The view should zoom into the center of the screen.  If your pitch is so that you see the sky in the center, there should not be any change when you try to zoom in.
- [ ] Just zooming in should not change the pitch or the heading (the compass should not change heading).

**Test 2: Zoom out**

**Steps:**
1. Click on the `ZoomOut` tool from the Navigation toolbar 
![image](https://devtopia.esri.com/storage/user/163/files/da367bfa-adcd-11e7-9ef2-284b1bde2771)

**Expectations**
- [ ] The view should zoom out from the center of the screen.  If your pitch is so that you see the sky in the center, there should not be any change when you try to zoom in.
- [ ] Just zooming out should not change the pitch or the heading (the compass should not change heading).

**Test 3: Set 2D view**
**Steps:**
1. Click on the `2D` button from the Navigation toolbar 
![image](https://devtopia.esri.com/storage/user/163/files/b8474448-b327-11e7-8fac-777a6a74f8bb)

**Expectations**
- [ ] The view should tilt to a vertical mode. 
- [ ] Zooming in/out should not change the pitch or the heading (the compass should not change heading).

**Test 4: Zoom in/out while following**

**Steps:**
1. Click on the `Follow` tool from the Navigation toolbar 
![image](https://devtopia.esri.com/storage/user/163/files/da367bfa-adcd-11e7-9ef2-284b1bde2771)
![image](https://devtopia.esri.com/storage/user/163/files/fcc4a62e-b327-11e7-9c35-7d568cdaae45)
2. Click on the start following icon:
![image](https://devtopia.esri.com/storage/user/163/files/242fed90-b328-11e7-95fb-a73059efaf53)
3. Click on zoom in/out button

**Expectations**
- [ ] The view should zoom in/out on the location symbol.  
- [ ] The camera should still follow the symbol until manually stopped.

**Test 5: Toolbar button toggle**

**Steps:**
1. Go to DSA > Settings
2. Toggle on/off "Show navigation Controls" check box

**Expectations:**
- [ ] It should hide/show the entire navigation toolbar which includes the compass 
![image](https://devtopia.esri.com/storage/user/163/files/8a107aee-adce-11e7-897c-ff1b5891ece8)
and the follow tool.
![image](https://devtopia.esri.com/storage/user/163/files/97731d22-adce-11e7-9551-974ba8b7fc0a)



## Location Text
- Elevation/Location text should display on the bottom
- By default it is DMS and in meters
- Go to settings, and toggle the visibility off. It should not appear
- Toggle visibility on again. It should appear
- Go to settings and select a different format and a different unit
- The text should update to use the selection
- Close the app and reopen. The same format/unit settings used in the previous run should be applied

## Context Menu
#### Test case 1: No menu shown for invalid context
- pitch the 3d view so that you can see the sky. Press and hold on the sky
- [ ] no context menu should be shown

#### Test case 2: Dismiss with no action
- Press and hold on the terrain
- [ ] the context menu should be shown
- click somewhere off the menu
- [ ] the menu should be dismissed and nothing should happen

#### Test case 3: Coordinates context
- press and hold on the terrain
- [ ] the context menu should be shown and should include the "Coordinates" option
- click "Coordinates"
- [ ] the coordinate conversion tool should open up with the context point as the input

#### Test case 4: Elevation context
- press and hold on the terrain
- [ ] the context menu should be shown and should include the "Elevation" option
- click "Elevation"
- [ ] a message box should appear showing the elevation (in meters) at the clicked position

#### Test case 5: Viewshed context
- press and hold on the terrain
- [ ] the context menu should be shown and include the "Viewshed" option
- click "Viewshed"
- [ ] a 360 degree viewshed should appear centered around the clicked position.
_Note - you can remove this viewshed with the Analysis List_

#### Test case 6: Identify context
- press and hold on a graphic (e.g. from a message feed)
- [ ] the context menu should be shown and include the "Identify" option
- click "Identify"
- [ ] a Popup should appear with the attributes of the graphic (it may not have any)
- press and hold on a feature (e.g. from a feature layer)
- [ ] the context menu should be shown and include the "Identify" option
- click "Identify"
- [ ] a Popup should appear with the attributes of the feature (it may not have any)

#### Test case 7: Follow context
- press and hold on a graphic (e.g. from a message feed)
- [ ] the context menu should be shown and include the "Follow" option
- click "Follow"
- [ ] The app should begin following the graphic
- interact with the view again to stop following

#### Test case 8: Line of sight context
- turn on location display
- press and hold on a graphic (e.g. from a message feed)
- [ ] the context menu should be shown and include the "Line of sight" option
- click "Line of sight"
- [ ] You should see a line of sight from the current location to the graphic you clicked on
_Note - you can remove this Line of sight with the Analysis List_

#### Observation
_See Observation Report section below_

### Identify Tool
- add some feature layer data (the bases.gdb is a good one)
- enable the identify/query tool and click on a graphic (the green geofence box has some attributes so you can use that)
  - [ ] confirm that you see a popup for the graphics attributes
- click on a feature
  - [ ] confirm that you see a popup for the feature's attributes
- click somewhere that a graphic and a feature overlap
  - [ ] confirm that you see a popup for both
- click somewhere with no graphic or feature
  - [ ] confirm there is no popup
  
### Display Current Location  
Setup:
- Make sure you have `MontereyMounted.gpx` (/Volumes/Data/sdk/qt/ExampleAppsData/DSA/) available in ~/ArcGIS/Runtime/Data/DSA. ***

Steps:
- Run the DSA app
- Make sure the app is in simulated location mode (it always is currently, it may change in the future).
- From the Navigation Toolbar, click on
![image](https://devtopia.esri.com/storage/user/798/files/b32ab67e-14f2-11e9-9a52-14f84e380662)
- Click on 
![image](https://devtopia.esri.com/storage/user/798/files/3c97681c-14f3-11e9-959e-02dc6e45bc4f)

    * The toolbar button will be underlined to indicate it's enabled now.
    * You should see an animated symbol moving along a track. 
    * The arrow point of the symbol should be facing in the direction of movement.
 - Zoom in and out very far while the symbol is moving. 
    * There should be no noticeable judder and the symbol should remain the same size until zoomed in very close. 
    * The symbol should remain visible while zoomed out and you can see the entire Globe (assumes 3D)
 - Rotate the View.
    * The arrow heading should always be pointing in the direction of movement, even as the view is rotated.
    * While rotating, the symbol should not flicker or have any undesirable side effects. It should remain consistent

- From the Navigation Toolbar, click again on
![image](https://devtopia.esri.com/storage/user/798/files/b32ab67e-14f2-11e9-9a52-14f84e380662)
   * The location symbol should be gone from the view as well.




## Follow Position
#### Setup:
- Make sure you have MontereyMounted.gpx (/Volumes/Data/sdk/qt/ExampleAppsData/DSA/) available in ~/ArcGIS/Runtime/Data/DSA/SimulationData. ***

#### Steps:
- Run the DSA app
Make sure the app is in simulated location mode (it always is currently, it may change in the future).
- From the Navigation Toolbar, click on
![image](https://devtopia.esri.com/storage/user/798/files/b32ab67e-14f2-11e9-9a52-14f84e380662)
- [ ] confirm that the "Follow Position" HUD buttons appear at the bottom of the screen
- there are 3 modes:
    - don't follow
    - follow (track up)
    - follow (north up)
- [ ] confirm that the app is initially in "don't follow" mode
- click the follow (track up) button
- [ ] confirm that the app begins to follow the simulated position and that it rotates to the current heading
- zoom in and out using the mouse wheel/navigation buttons. 
- [ ] confirm that the app is still following
- interact with the view in some way (e.g. pan, pitch or heading)
- [ ] confirm that the app immediately comes out of follow mode and goes back to don't follow state
- click the follow (north up) button
- [ ] confirm that the app starts to follow the position, is pitched to a map view perspective and does not rotate (e.g. north is top)
- zoom in and out
- [ ] confirm that the app is still following
- interact with the view in some way (e.g. pan, pitch or heading)
- [ ] confirm that the app immediately comes out of follow mode and goes back to don't follow state
- toggle through the states to check it's all changing as expected
- from the Navigation Toolbar, click again on
![image](https://devtopia.esri.com/storage/user/798/files/b32ab67e-14f2-11e9-9a52-14f84e380662) (e.g. turn off position)
- [ ] confirm that the app stops following and the follow HUD disappears
### Coordinate Conversion Tool
## Test case 1: Default coordinate format
- start the app and open the coordinate conversion tool (using the tool icon on the Map toolbar)
- [ ] the tool should appear at the bottom of the screen
- [ ] the tool should be using MGRS format as the input
- [ ] the tool should be tracking the app's current position
- shut the tool

## Test case 2: Launch from context menu
- long press on the terrain. Click "Coordinates" on the context menu
- [ ] the tool should appear (and should be shown as selected in the Map toolbar)
- [ ] the tool should show the position where you launched the context-menu (you can use the flash coordinate button to check)
- [ ] the tool should be in capture mode (not tracking the apps position) - you can expand the tool to check
- long press on the view again while the coordinate too is open
- [ ] the context menu should not appear

## Test case 3: Flash position in view
- open the tool in capture mode and click in the view to set a position
- click the flash in view button
- [ ] you should see a point flash in the view at the clicked position

## Test case 4: Go to position
- open the tool in live mode
- click the Go to coordinate button
- [ ] the view should zoom to the current position of the app (you can confirm by turning on the location didsplay)
-----------------------------------------------------




# 4. Message Feeds
## Broadcast Current Location
The current location updates will be enabled in the DSA app at startup.

**Test 1: General broadcast location test**

- Connect 2 or more devices to the same network.  Either via LAN connection or to the Watchtower wifi (Esri-internal wifi has the broadcast ports blocked, so don't use it)
- Each app should display a military symbol for each others location at the default frequency of 3000ms updates.

**Test 2: Disable/Enable location broadcast**
- In one of the apps, locate the `Settings` panel and un-check the "Location Broadcast checkbox.
- Notice in the other app that the military symbol stops updating and remains where it left off.
- Wait for several seconds and enable the checkbox again.
- Now the military symbol should begin moving again at the default frequency of 3000ms updates.

**Test 3: Modify Location Broadcast frequency**
- In one of the apps, locate the `Settings` panel and change the "Location Broadcast Frequency" value from `3000`ms to `100`ms.
- In the other app you will notice that the military symbol is updating its location much more frequently than before.

**Test 4: Show teammate in distress**
- Locate the distress icon/button in the app
- Press the button and the icon should change to indicate that the app is now broadcasting a "in distress" status.
- From the other app, notice that the teammate's military symbol is now flashing red.
- Press the distress button again and the teammate's military symbol should no longer be flashing red and should no longer be selected in any way.

**Test 5: Remove location broadcast**
- Close down one of the apps
- You should notice that the teammate's military symbol is now gone and have been removed from the message feed overlay.


# 5. Observation Reports
#### Test case 1: Create Observation Report from Tool
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

#### Test case 2: Create Observation Report from Context Menu
- long press on the view to open the Context Menu
  - [ ] you should see an option for Observation Report
- select the option
  - [ ] the Contact Report should open and you should see the location graphic indicating the osition you launched the context menu from
- step through the wizard. On the location page use the current position icon (looks like a GPS cross-hair) to update the position to the current app location
  - [ ] the location should update the next time the apps position is updated (you may need to pan to find it)
- run through the rest of the wizard. On the final page click Cancel
  - [ ] the report should be cleared and the location highlight graphic should disappear

# 6. Analysis Tools
## Line of Sight
#### Test case 1: Enable Line of Sight
- turn on the location display
- go to the analysis tab and select "Line of sight"
- select "Observers_SD" from the overlay drop down
  - [ ] line of sight should be displayed from the various observer positions (atop buildings). 
  - [ ] Should look something like this:
#### Test case 2: Toggle visibility
- uncheck the box next to the overlay drop down
  - [ ] the line of sight should disappear
- recheck the box
 - [ ] they should reappear

#### Test case 3: Remove
- click the "x" button next to the overlay drop down
  - [ ] the lines of sight should disappear and the overlay drop down should be cleared

## Viewshed
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



# 7. Alerts and Conditions
#### Setup
- delete the `ArcGIS\Runtime\Data\DSA\DsaAppConfig.json` (to make sure you get the default distress condition)
- you will need to run the `ArcGIS\Runtime\Data\DSA\SimulationData\GeoMessage_FriendlyTracksLand.xml` tracks to simulate a distress message. You can run this using the simulator @ `\\apps-data\data\sdk\qt\exampleApps\Simulators\DSA_MessageSimulator_Qt`
- you will need to use the `Cot_Reports.xml` which can be run with the `DSA_MessageSimulator_Qt` app

#### Test case 1: default distress condition (attribute equals)
- start the app
- [ ] a new `DsaAppConfig.json` file should be created and it should contain a Condition called "Distress"
- start the simulator and run the `GeoMessage_FriendlyTracksLand.xml` file
- [ ] go to the Alert Conditions tool. You should see the "Distress" condition. 
- close the Alert Conditions tool
- [ ] wait for a while and you should see an alert notification for the distress condition (a small red circle with the number of alerts).
- open the alert list tool
- [ ] the notification should disappear
- [ ] zoom to the alert and the app should zoom in on the track which is in distress
- [ ] highlight the alert and you should see the highlight graphic centered on the track
- [ ] set highlight off and the highlight graphic should disappear
- wait for a bit longer
- [ ] you should see the alert disappear when the track is no longer in distress
- wait for a bit longer
- [ ] you should see another notification when the alert moves back into the distress state

#### Test case 2: creating/editing alerts (within distance)
- restart the app
- turn on location display (should be from simulated GPS file)
- open the markup tool and draw a big "X" over Monterey (see screenshot)

![image](https://devtopia.esri.com/storage/user/1340/files/b8e09322-1183-11e8-8acc-626e91e0687f)

- open the Alerts Condition tool and create a new moderate priority Geofence condition called "test condition" where "My Location" is within 250 meters of "Sketch Overlay"
- [ ] when the location display gets within 250 m of the X an alert should be created
_NOTE whenever the location moves out of the 250 m zone the alert should disappear, and the following steps will not be possible_
- in the alerts view tool set the Minimum level to "High"
- [ ] the alert should disappear
- [ ] set the minimum level back to Low and the alert should disappear again
- [ ] dismiss the alert and it should disappear
- [ ] click "Clear Filters" and the alert should re-appear
- go to the Conditions tab and un-check/disable the condition you created
- [ ] the associated alert should disappear from the list
- [ ] check/enable the condition again and the alert should reappear
- [ ] Edit the condition (e.g. change it's name/level) and the associated alert should get a new name/level as well
- wait until the location overlay moves away from the "X" you drew and the alert should disappear
- open the `DsaAppConfig.json` file and confirm that their is a json representation of the condition you just created

#### Test case 3: pick specific feature for condition (within area)
- re-start the app
- add the `AOI.shp` layer
- start the `DSA_MessageSimulator_Qt` app using `Cot_Reports.xml`
- create a new Critical Priority Geofence condition when objects from Cursor-on-Target are within Object 0 from AOI. To set the specific polygon to use, toggle the pick tool and click on the polygon in the screenshot below:

![image](https://devtopia.esri.com/storage/user/1340/files/e3faf8d4-1185-11e8-9cfe-0eeecf992371)

- [ ] wait until one of the tracks appears in the target polygon and you should see an alert created
- [ ] the track in the area should flash continuously while it is in the alert state
- [ ] go to the conditions list and disable the condition you added. the track should stop flashing
- [ ] re-enable the condition and the track should start flashing again


# 8. Markup Tool
Run the DSA apps

#### Test 1: Draw multiple graphics
- Go to Markup
- Begin sketching. Draw should be enabled by default, so as soon as you are in the Markup group, you should be able to sketch.
- Sketch another couple of lines
- Navigate to a different category (like map), and drawing should not be enabled

#### Test 2: Configure markup color
- Go to markup
- Start sketching some lines
- Click color and select a different color. 
- Sketch a new line. The new sketch should use the new color

#### Test 3: Share Markup
- Go to markup
- Start sketching some lines
- Click color and select a different color. 
- Sketch a new line. The new sketch should use the new color
- Press Share
- Give the sketch a name and press OK
- On your device, you should get a prompt saying that you shared a sketch, and then asking if you would like to view it as a layer. Press Yes
- Go to Maps > Table of Contents, and you should see your markup added as a layer

#### Test 4: Receive
- Run the app from 2 different devices connected to the watchtower wifi (or other non Esri wifi -- a wireless hotspot from your phone works well)
- In one of the apps, Go to Markup
- Start Sketching
- Press Share
- Give the markup a name and press OK
- On your second device, you should get a prompt saying that someone has sent you a sketch, and asks if you would like to view it
- Select yes, and it will add it as a layer
- Go to Maps section > Table of Contents tool, and you should see your sketch

#### Test 5: Manually add layer
- Go to Maps > Add Local Data tool
- Filter the data type by Markup (.markup)
- Select one of the markups you created in the previous steps
- It should add the markup as a layer
- Close and reopen the app, and the markup should persist




# 9. App Config and Settings
## DsaConfig file
#### Setup
delete the json settings file from `~\ArcGIS\Runtime\Data\DSA\DsaAppConfig.json`

#### Test 1: default values written to json
- [ ] when you start the app you get default values written to the json file for:
- location text format (e.g. DMS)
- a set of message feeds (e.g. friendly tracks)
- alert distress condition
- initial location
- basemap directory and default basemap
- default elevation source
- gps file

#### Test 2: json values used by the app
- [ ] when you start the app:
 - it starts in the expected default location (Monteray)
 - it has a basemap
 - it has an elevation source
 - the message feeds tool contains the names of feeds
 - the location text has the expected format

#### Test 3: json values are written by the app
- [ ] when you change a setting it is saved to the json file and applied when you restart the app
 - the format for location text
 - the default basemap
 - a condition

## Persist Layers
- Open the DSA app
- Go to the add data tool and add several layers
- Go to the Overlays/TOC tool, and those layers should be reflected
- Close/Reopen the app, go to the Overlays/TOC, and those same layers should be in the list and display on the screen
- Turn the visibility of one of the layers off, and reorder the layers (move one of them up or down)
- Close/Reopen the app, go to the Overlays/TOC tool, and the visibility and layer order should be retained

## Settings
## App Configuration

# App and Tool Errors
#### Test Case 1: No errors when everything is present
- open the app
- [ ] there should be no error messages shown at start up
- play around with some tools etc.
- [ ] you should see no errors (unless you do something invalid)

#### Test Case 2: No Data
- rename the "DSA" folder to "DSA2"
- restart the app
- [ ] you should see an error message reporting No Data
- change the folder back to DSA and run the app again
- [ ] you should not see an error

#### Test Case 3: Error on startup
- rename `~\ArcGIS\Runtime\Data\DSA\BasemapData\Topographic.tpk` to `Topographic2.tpk`
- open the app
- [ ] you should see an error on startup informing you that the default basemap could not be found
_NOTE - the error may be duplicated (I think that's ok)_
- change the file back to `Topographic.tpk` and reopen the app
- [ ] you should not see an error

#### Test Case 4: Error using tool
_this test is for errors we create at the app level_
- go to the create alerts condition tool and create a new Attribute alert using "My Location" as the source feed (this is invalid since the location has no attributes)
- [ ] you should see an error indicating that the condition was not created

#### Test Case 5: Error from the API
_this test is for errors we receive from the API_
- rename `~\ArcGIS\Runtime\Data\DSA\OperationalData\AOI.dbf` to `AOI2.dbf` (this will make the AOI shapefile invalid when we try to add it)
- go to the add data tool and select the `AOI.shp` to add
- [ ] you should see an error
