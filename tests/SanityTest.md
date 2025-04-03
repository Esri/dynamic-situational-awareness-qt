Sanity test list to perform on several platforms prior to release. 

# 1. Setup / Test Data
#### General comments
- Test the app on at least on Windows and Android
  
#### Prepare device for tests
- Delete (or rename) DSA data folder (/ArcGIS/Runtime/Data) so that it can be recreated
  - On Android, do this by opening the App Info (long press on the app icon and choose 'info'), scroll down and tap on Storage, tap Clear data.
- Open the DSA app
  - [ ] You should be prompted to download the default configuration from Esri.  Click Yes
- Click OK on the Error dialog that the GPX file is missing
  - [ ] You should be brought to the Configurations page and see the data downloading
- Wait for the data to download. Once it's done downloading, tap "Close App" (the app needs to reload to read the data)
- tap "OK" to confirm closing the app
- Open the app
  - [ ]  Confirm the default data is shown in the app (topo basemap is drawn, datasets are available in `Local Data` panel)

# 2. Map Toolbar
## Basemap Picker
- Run the DSA app
  - [ ] confirm that the app starts with the topographic basemap
- Select the basemap tool
  - [ ] confirm that you see a list of all the .tpk files in the BasemapData folder (default is `ImageryMonterey` & `Topographic`)
- select one of the tpks
  - [ ] confirm that the tool closes and the basemap changes
- reopen the tool
- click the close button (`X`)
  - [ ] confirm that the tool closes without changing the basemap   
DONE

## Add Local Data
**Setup:**
> You can use the default demo data for this test, but it has a limited set of data. Ideally, copy additional datasets into the OperationalData folder so you can test all types. 

**Steps:**
- Run the DSA app
- Select Map Tools icon, then the Add Data tool and change the filter to `Geodatabase`
- Check the box for one of the geodatabase files and click Add Selected
  - [ ] Confirm the data is displayed on the scene. 
- Select the Add Data tool again and change the filter to `Shapefile`
- Check the box for one of the shapefiles and click Add Selected
  - [ ] Confirm the shapefile is added to the scene 
- Select the  the Add Data tool again, and change the filter to `Raster Files`
- Check the box for a .dt2 file
- Select the 'Add as elevation source' checkbox and click ADD SELECTED
  - [ ] Confirm the DTED file is added as an elevation source to the scene (you should see the terrain surface reload)
DONE  

> _In preparation for the next test, do NOT remove the layers from the Overlays list_

## Overlays
> _If not done already, run the previous test (`Add Local Data`) to make sure there is data load in the app_

**Steps:**
- Run the DSA app
- Open the Overlays tool
  - [ ] (VEHICLE) confirm that you can still interact with the scene 
  - [ ] confirm that the layers you added are in the list
  - [ ] confirm that each layer has an icon that matches it's type (points, lines, polygons, raster)
  - [ ] confirm that each layer has a name (there is a known issue about names for raster layers so we have a workaround for now)
- for each layer in turn: 
  - click on the Ellipses and choose to zoom to the layer
    - [ ] confirm the scene zooms  
  - Open Overlays again 
  - Uncheck one of the layers
  - (ANDROID) Close Overlays
    - [ ] confirm the layer is _not_ displayed on the scene
  - (ANDROID) Open Overlays
- experiment with reordering the layers by selecting the ellipses and selecting move up or down
  - [ ] confirm that reordering the list also reorders the layers in the map/scene
- remove a layer by selecting the ellipses
  - [ ] confirm the layer is removed from the list
  - [ ] confirm the layer is removed from the map/scene
- close the tool   
DONE

## Message Feeds
> The DSA app by default should be configured to handle certain message feeds.
> They are:
> 1.  SA Events - (Cursor-on-Target)
> 2.  Friendly Tracks - Land (GeoMessage)
> 3.  Friendly Tracks - Air (GeoMessage)
> 4.  Contact Reports (GeoMessage)
> 5.  Situation Reports (GeoMessage)
> 6.  EOD Reports (GeoMessage)
> 7.  Sensor Observations (GeoMessage)

To test on a mobile device, you may need to turn off VPN if you have it on, or update your network settings to allow traffic through the UDP port.

**Setup**
- Run the DSA app
- (WINDOWS) Run the simulators using the StartDSATracks.bat

**Test 1: Turn on/off message feeds**
- Run the DSA app (Vehicle or Handheld)
- [ ] You should see data from the message feeds on the scene (blue military symbols moving, plus other point symbols appearing):
  - [ ] `SA Events` message feed should draw red and green military symbols, billboarded on the view
  - [ ] `Friendly Tracks - Land` message feed should begin drawing mil2525c friendly (blue) billboarded (lollipop) symbols on the view
  - [ ] `Friendly Tracks - Air` message feed should begin drawing mil2525c friendly (blue) symbols to the screen in absolute surface placement mode.  The symbols are floating in the air at a fixed elevation.
  - [ ] `Contact Reports`, `Situation Reports`, `EOD Reports`, and `Sensor Observations` message feeds should begin drawing picture marker symbol types (gray and white) to the view for each feed.
- Select the `Message Feeds` panel and toggle off each one of the message feeds:
  - [ ] The `SA Events` graphics (green and red symbols) should no longer be drawn to the view.
  - [ ] The `Friendly Tracks - Land` graphics should no longer be drawn to the view.
  - [ ] The `Friendly Tracks - Air` graphics should no longer be drawn to the view.
  - [ ] The `Contact Reports`, `Situation Reports`, `EOD Reports`, and `Sensor Observations` message feeds graphics should no longer be drawn to the view.

**Test 2: Toggle Friendly Tracks labels**
- While the DSA app and `GeoMessage_FriendlyTracksLand.xml` simulation running, select the `Message Feeds` panel and make sure the `Friendly Tracks - Land` message feed is enabled.
- [ ] Confirm that labels are displayed next to the blue graphics.
- Open up the settings panel and uncheck the "Show friendly tracks labels" checkbox.
- [ ] Confirm that labels are not displayed anymore next to the blue graphics.
- Check the "Show friendly tracks labels" checkbox back on.
- [ ] Confirm that labels are displayed next to the blue graphics again.

DONE

## Open Package
Setup:
- Make sure you have a mobile scene package to test with in the Packages folder

Steps:
- Open DSA
- Tap the `Open` tool on the Map toolbar
  - [ ] Confirm you see a mobile scene package listed
- Tap the mobile scene package, and then select a Scene
  - [ ]  Confirm it opens the scene (you may need to zoom in to see the basemap, or turn one on in the Basemaps panel)
- Open the Overlays panel
  - [ ] Confirm you see a list of layers from the scene package listed, and that you can interact with the like any other layer (see previous test steps)
- Open the Add Local Data panel and add another layer to the scene
  - [ ] Confirm it adds the layer to the Overlays panel
- Restore back to the 'default' scene by tapping the `Open` tool on the Map toolbar, then the `Packages` button, and then the `Reset to Default Scene` button
  - [ ] Confirm you see the default basemap shown and no other overlays in the Overlay list. (everything in the map should be reset)

# 3. Map Display Tools
## Navigation Tools
Run the DSA app

**Test 1: Zoom in**

**Steps:**
1. Click on the `ZoomIn` tool from the Navigation toolbar 
![image](https://github.com/user-attachments/assets/b743ac50-8b16-4d36-ba9d-fbdf7f8b7929)


**Expectations**
- [ ] The view should zoom into the center of the screen.  
- [ ] Just zooming in should not change the pitch or the heading (the compass should not change heading).

**Test 2: Zoom out**

**Steps:**
1. Click on the `ZoomOut` tool from the Navigation toolbar 
![image](https://github.com/user-attachments/assets/0e205aa0-657e-47ec-8823-3ee3ce8e3c86)


**Expectations**
- [ ] The view should zoom out from the center of the screen.  If your pitch is so that you see the sky in the center, there should not be any change when you try to zoom in.
- [ ] Just zooming out should not change the pitch or the heading (the compass should not change heading).

**Test 3: Set 2D view**
**Steps:**
1. Click on the `2D` button from the Navigation toolbar 
![image](https://github.com/user-attachments/assets/983a2917-ee16-46f8-a254-169cf6760b4b)


**Expectations**
- [ ] The view should tilt to a vertical mode. 
- [ ] Zooming in/out should not change the pitch or the heading (the compass should not change heading).

**Test 4: Zoom in/out while following**

**Steps:**
1. Click on the `Location` tool from the Navigation toolbar 
![image](https://github.com/user-attachments/assets/e1a69005-8061-4aa1-a0a7-276bddf5fba9)
2. Click on the start following icon:
![image](https://github.com/user-attachments/assets/63d0314a-296f-46cc-a41f-98555e871923)  
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
![image](https://github.com/user-attachments/assets/95a6f9f3-c7e8-48f4-a382-6298e330df93)

and the follow tool.
![image](https://github.com/user-attachments/assets/51b3c3cc-783d-483d-a02e-319b1b758860)




## Location Text
- Elevation/Location text should display on the bottom
- By default it is MGRS and in meters
- Go to settings, and toggle the visibility off. It should not appear
- Toggle visibility on again. It should appear
- Go to settings and select a different format and a different unit
- The text should update to use the selection
- Close the app and reopen. The same format/unit settings used in the previous run should be applied

## Context Menu
Test case 1: No menu shown for invalid context
- pitch the 3d view so that you can see the sky. Press and hold on the sky
- [ ] no context menu should be shown

Test case 2: Dismiss with no action
- Press and hold on the terrain
- [ ] the context menu should be shown
- click somewhere off the menu
- [ ] the menu should be dismissed and nothing should happen

Test case 3: Coordinates context
- press and hold on the terrain
- [ ] the context menu should be shown and should include the "Coordinates" option
- click "Coordinates"
- [ ] the coordinate conversion tool should open up with the context point as the input

Test case 4: Elevation context
- press and hold on the terrain
- [ ] the context menu should be shown and should include the "Elevation" option
- click "Elevation"
- [ ] a message box should appear showing the elevation (in meters) at the clicked position

Test case 5: Viewshed context
- press and hold on the terrain
- [ ] the context menu should be shown and include the "Viewshed" option
- click "Viewshed"
- [ ] a 360 degree viewshed should appear centered around the clicked position.
_Note - you can remove this viewshed with the Analysis List_

Test case 6: Identify context
- press and hold on a graphic (e.g. from a message feed)
- [ ] the context menu should be shown and include the "Identify" option
- click "Identify"
- [ ] a Popup should appear with the attributes of the graphic (it may not have any)
- press and hold on a feature (e.g. from a feature layer)
- [ ] the context menu should be shown and include the "Identify" option
- click "Identify"
- [ ] a Popup should appear with the attributes of the feature (it may not have any)

Test case 7: Follow context
- press and hold on a graphic (e.g. from a message feed)
- [ ] the context menu should be shown and include the "Follow" option
- click "Follow"
- [ ] The app should begin following the graphic
- interact with the view again to stop following

Test case 8: Line of sight context
- turn on location display
- press and hold on a graphic (e.g. from a message feed)
- [ ] the context menu should be shown and include the "Line of sight" option
- click "Line of sight"
- [ ] You should see a line of sight from the current location to the graphic you clicked on
_Note - you can remove this Line of sight with the Analysis List_

### Observation
_See Observation Report section below_


  
### Display Current Location  
Setup:
- Make sure you have `MontereyMounted.gpx` available in ~/ArcGIS/Runtime/Data/DSA. ***

Steps:
- Run the DSA app
- Make sure the app is in simulated location mode (it always is currently, it may change in the future).
- From the Navigation Toolbar, click on ![image](https://github.com/user-attachments/assets/95f6aa23-cef1-405c-900f-f6025187eee9)

- Click on ![image](https://github.com/user-attachments/assets/c64ff247-1c59-446c-8883-7cd8b45f922e)
  * The toolbar button will be underlined to indicate it's enabled now.
  * You should see an animated symbol moving along a track. 
  * The arrow point of the symbol should be facing in the direction of movement.
 - Zoom in and out very far while the symbol is moving. 
  * There should be no noticeable judder and the symbol should remain the same size until zoomed in very close. 
  * The symbol should remain visible while zoomed out and you can see the entire Globe (assumes 3D)
 - Rotate the View.
  * The arrow heading should always be pointing in the direction of movement, even as the view is rotated.
  * While rotating, the symbol should not flicker or have any undesirable side effects. It should remain consistent

- From the Navigation Toolbar, click again on ![image](https://github.com/user-attachments/assets/55d84b24-a29e-49ee-a33b-0f814d5ebbd5)
  * The location symbol should be gone from the view as well.


## Follow Position
Setup:
- Make sure you have MontereyMounted.gpx available in ~/ArcGIS/Runtime/Data/DSA/SimulationData.

Steps:
- Run the DSA app
Make sure the app is in simulated location mode (it always is currently, it may change in the future).
- From the Navigation Toolbar, click on ![image](https://github.com/user-attachments/assets/517876e8-db78-4dde-9b94-794771d5650b)
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
- from the Navigation Toolbar, click again on ![image](https://github.com/user-attachments/assets/517876e8-db78-4dde-9b94-794771d5650b) (e.g. turn off position)
- [ ] confirm that the app stops following and the follow HUD disappears

## Coordinate Conversion Tool
Test case 1: Default coordinate format
- start the app and open the coordinate conversion tool (using the tool icon on the Map toolbar)
- [ ] the tool should appear at the bottom of the screen
- [ ] the tool should be using MGRS format as the input
- [ ] the tool should be tracking the app's current position
- shut the tool

Test case 2: Launch from context menu
- long press on the terrain. Click "Coordinates" on the context menu
- [ ] the tool should appear (and should be shown as selected in the Map toolbar)
- [ ] the tool should show the position where you launched the context-menu (you can use the flash coordinate button to check)
- [ ] the tool should be in capture mode (not tracking the apps position) - you can expand the tool to check
- long press on the view again while the coordinate too is open
- [ ] the context menu should not appear

Test case 3: Flash position in view
- open the tool in capture mode and click in the view to set a position
- click the flash in view button
- [ ] you should see a point flash in the view at the clicked position

Test case 4: Go to position
- open the tool in live mode
- click the Go to coordinate button
- [ ] the view should zoom to the current position of the app (you can confirm by turning on the location display)





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
Test case 1: Create Observation Report from Tool
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

Test case 2: Create Observation Report from Context Menu
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
Test case 1: Enable Line of Sight
- turn on the location display
- go to the analysis tab and select "Line of sight"
- select "Observers_SD" from the overlay drop down
  - [ ] line of sight should be displayed from the various observer positions (atop buildings). 
  - [ ] Should look something like this:
Test case 2: Toggle visibility
- uncheck the box next to the overlay drop down
  - [ ] the line of sight should disappear
- recheck the box
 - [ ] they should reappear

Test case 3: Remove
- click the "x" button next to the overlay drop down
  - [ ] the lines of sight should disappear and the overlay drop down should be cleared

## Viewshed
Test case 1: Current Location viewshed
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

Test case 2: Map Element viewshed
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

Test case 3: Follow Touch viewshed
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

Test case 4: Manage viewsheds
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
Setup
- delete the `ArcGIS\Runtime\Data\DSA\Default\DsaAppConfig.json` (to make sure you get the default distress condition)
- make sure the simulator is running

Test case 1: default distress condition (attribute equals)
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

Test case 2: creating/editing alerts (within distance)
- restart the app
- turn on location display (should be from simulated GPS file)
- open the markup tool and draw a big "X" over Monterey (see screenshot)
![image](https://github.com/user-attachments/assets/cf510d8b-8469-4cb0-bd43-bc2cb6fdc9fa)
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

Test case 3: pick specific feature for condition (within area)
- re-start the app
- add the `AOI.shp` layer
- start the `DSA_MessageSimulator_Qt` app using `Cot_Reports.xml`
- create a new Critical Priority Geofence condition when objects from Cursor-on-Target are within Object 0 from AOI. To set the specific polygon to use, toggle the pick tool and click on an AOI polygon
- [ ] wait until one of the tracks appears in the target polygon and you should see an alert created
- [ ] the track in the area should flash continuously while it is in the alert state
- [ ] go to the conditions list and disable the condition you added. the track should stop flashing
- [ ] re-enable the condition and the track should start flashing again


# 8. Markup Tool
Run the DSA apps

Test 1: Draw multiple graphics
- Go to Markup
- Begin sketching. Draw should be enabled by default, so as soon as you are in the Markup group, you should be able to sketch.
- Sketch another couple of lines
- Navigate to a different category (like map), and drawing should not be enabled

Test 2: Configure markup color
- Go to markup
- Start sketching some lines
- Click color and select a different color. 
- Sketch a new line. The new sketch should use the new color

Test 3: Share Markup
- Go to markup
- Start sketching some lines
- Click color and select a different color. 
- Sketch a new line. The new sketch should use the new color
- Press Share
- Give the sketch a name and press OK
- On your device, you should get a prompt saying that you shared a sketch, and then asking if you would like to view it as a layer. Press Yes
- Go to Maps > Table of Contents, and you should see your markup added as a layer

Test 4: Receive
- Run the app from 2 different devices connected to the watchtower wifi (or other non Esri wifi -- a wireless hotspot from your phone works well)
- In one of the apps, Go to Markup
- Start Sketching
- Press Share
- Give the markup a name and press OK
- On your second device, you should get a prompt saying that someone has sent you a sketch, and asks if you would like to view it
- Select yes, and it will add it as a layer
- Go to Maps section > Table of Contents tool, and you should see your sketch

Test 5: Manually add layer
- Go to Maps > Add Local Data tool
- Filter the data type by Markup (.markup)
- Select one of the markups you created in the previous steps
- It should add the markup as a layer
- Close and reopen the app, and the markup should persist




# 9. App Config and Settings
## DsaConfig file
Setup
delete the json settings file from `~\ArcGIS\Runtime\Data\DSA\Default\DsaAppConfig.json`

Test 1: default values written to json
- [ ] when you start the app you get default values written to the json file for:
- location text format (e.g. DMS)
- a set of message feeds (e.g. friendly tracks)
- alert distress condition
- initial location
- basemap directory and default basemap
- default elevation source
- gps file

Test 2: json values used by the app
- [ ] when you start the app:
 - it starts in the expected default location (Monteray)
 - it has a basemap
 - it has an elevation source
 - the message feeds tool contains the names of feeds
 - the location text has the expected format

Test 3: json values are written by the app
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

# 10. App and Tool Errors
Test Case 1: No errors when everything is present
- open the app
- [ ] there should be no error messages shown at start up
- play around with some tools etc.
- [ ] you should see no errors (unless you do something invalid)

Test Case 2: No Data
- rename the "DSA" folder to "DSA2"
- restart the app
- [ ] you should see an error message reporting No Data
- change the folder back to DSA and run the app again
- [ ] you should not see an error

Test Case 3: Error on startup
- rename `~\ArcGIS\Runtime\Data\DSA\Default\BasemapData\Topographic.tpk` to `Topographic2.tpk`
- open the app
- [ ] you should see an error on startup informing you that the default basemap could not be found
_NOTE - the error may be duplicated (I think that's ok)_
- change the file back to `Topographic.tpk` and reopen the app
- [ ] you should not see an error

Test Case 4: Error using tool
_this test is for errors we create at the app level_
- go to the create alerts condition tool and create a new Attribute alert using "My Location" as the source feed (this is invalid since the location has no attributes)
- [ ] you should see an error indicating that the condition was not created

Test Case 5: Error from the API
_this test is for errors we receive from the API_
- rename `~\ArcGIS\Runtime\Data\DSA\Default\OperationalData\AOI.dbf` to `AOI2.dbf` (this will make the AOI shapefile invalid when we try to add it)
- go to the add data tool and select the `AOI.shp` to add
- [ ] you should see an error
