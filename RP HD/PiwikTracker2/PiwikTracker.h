//
//  PiwikTracker.h
//  PiwikTracker
//
//  Created by Mattias Levin on 3/12/13.
//  Copyright 2013 Mattias Levin. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AFNetworking.h"


/**
 
 The PiwikTracker is an Objective-C framework for sending analytics to a Piwik server.
 
 Piwik server is downloadable, Free/Libre (GPLv3 licensed) real time web analytics software, [http://piwik.org](http://piwik.org).
 This framework implements the Piwik tracking REST API [http://piwik.org/docs/tracking-api/reference.](http://piwik.org/docs/tracking-api/reference/)
 
 ###How does it work
 
 1. Create and configure the tracker
 2. Track screen views, events and goals
 3. Let the dispatch timer dispatch pending events to the Piwik server or start the dispatch manually


 All events are persisted locally in Core Data until they are dispatched and successfully received by the Piwik server.
 
 All methods are asynchronous and will return immediately.
 */
@interface PiwikTracker : AFHTTPClient


/**
 @name Creating a Piwik tracker
 */

/**
 Create and configure a shared Piwik tracker.
 
 @param baseURL The base URL of the Piwik server. The URL should not include the tracking endpoint path component (/piwik.php)
 @param siteID The unique side id generated by the the Piwik server when the tracked website/application is created
 @param authenticationToken The unique authentication token generated by the the Piwik server when the tracked website/application is created
 @return The newly created PiwikTracker object
 */
+ (instancetype)sharedInstanceWithBaseURL:(NSURL*)baseURL siteID:(NSString*)siteID authenticationToken:(NSString*)authenticationToken;

/**
 Return the shared Piwik tracker.
 
 The Piwik tracker must have been created and configured for this method to return the tracker.
 
 @return The existing PiwikTracker object
 @see sharedInstanceWithBaseURL:siteID:authenticationToken:
 */
+ (instancetype)sharedInstance;

/**
 Piwik site id generated by the Piwik server.
 */
@property (nonatomic, readonly) NSString *siteID;

/**
 Piwik authentication token generated by the Piwik server.
 */
@property (nonatomic, readonly) NSString *authenticationToken;

/**
 Unique client id, used to identify unique visitors.
 
 This id is generated the first time the app is installed. The value will be retained across app restart and upgrades. If the application uninstalled and installed again, a new value will be generated. Requires the authentication token to be set.
 */
@property (nonatomic, readonly) NSString *clientID;


/**
 @name Tracker configuration
 */

/**
 Run the tracker in debug mode.
 
 Instead of sending events to the Piwik server, events will be printed to the console. Can be useful during development.
 */
@property(nonatomic) BOOL debug;

/**
 Opt out of tracking.
 
 No events will be sent to the Piwik server. This feature can be used to allow the user to opt out of tracking due to privacy. The value will be retained across app restart and upgrades.
 */
@property(nonatomic) BOOL optOut;

/**
 The probability of an event actually being sampled and sent to the Piwik server. Value 1-100, default 100.
 
 Use the sample rate to only send a sample of all events generated by the app, this can be useful for applications that generate a lot of events.
 */
@property (nonatomic) double sampleRate;

/**
 Events sent to the Piwik server will include the users current position when the event was generated. This can be used to improve plotting of visitors location. Default NO. This value must be set before the tracker is used the first time.
 
 Think about users privacy. Provide information why their location is tracked and give them an option to opt out.
 
 Turning this ON will potentially use more battery power. The tracker will only react to significant location changes to reduce battery impact. Location changes will not be tracked when the app is terminated or running in the background.
 Users can decided to not allow the app to access location information.
 */
@property (nonatomic) BOOL includeLocationInformation;


/**
 @name Session control
 */

/**
 Set this value to YES to force a new session start when the next event is sent to the Piwik server.
 
 By default a new session is started each time the application in launched.
 */
@property (nonatomic) BOOL sessionStart;

/**
 A new session will be generated if the application spent longer time in the background then the session timeout value. Default value 120 seconds.
 
 The Piwik server will also generate a new session if the event is recorded 30 minutes after the previous received event. Requires the authentication token to be set.
 */
@property (nonatomic) BOOL sessionTimeout;


/**
 @name Track screen views, events and goals
 */

/**
 Track a single screen view.
 
 @param screen The name of the screen to track.
 @return YES if the event was queued for dispatching.
 */
- (BOOL)sendView:(NSString*)screen;

/**
 Track a single screen view.
 
 Piwik support hierarchical screen names, e.g. /settings/register. Us this to group and categorise events.
 
 @param screen A list of names of the screen to track.
 @param ... A list of names of the screen to track.
 @return YES if the event was queued for dispatching.
 */
- (BOOL)sendViews:(NSString*)screen, ...;

/**
 Track an event (as oppose to a screen view).
 
 Events are tracked as hierarchical screen names, category/action/label.
 @param category The category of the event
 @param action The action name
 @param label The label name
 @return YES if the event was queued for dispatching.
 */
- (BOOL)sendEventWithCategory:(NSString*)category action:(NSString*)action label:(NSString*)label;

/**
 Track a goal conversion.
 
 @param goalID The unique goal ID as configured in the Piwik server.
 @param revenue The monetary value of the conversion.
 @return YES if the event was queued for dispatching.
 */
- (BOOL)sendGoalWithID:(NSString*)goalID revenue:(NSUInteger)revenue;


/**
 @name Dispatch pending events
 */

/**
 The tracker will automatically dispatcher all pending events on timer. Default value 120 seconds.
 
 If a negative value is sent the dispatch timer will never run and manual dispatch must be used @see dispatch. If 0 is set the event is dispatched as as quick as possible after it has been queued.
 
 */
@property(nonatomic) NSTimeInterval dispatchInterval;

/**
 Specifies the maximum number of events queued in core date. Default 500.
 
 If the number of queued events exceed this value events will no longer be queued.
 */
@property (nonatomic) NSUInteger maxNumberOfQueuedEvents;

/**
 Specifies how many events should be sent to the Piwik server in each request. Default 20 event per request.
 
 The Piwik server support sending one event at the timer (as a HTTP GET operation with the event parameters as the query string) or in bulk mode (as a HTTP POST operation with the events JSON encoded). Requires the authentication token to be set to send more the one event at the time.
 */
@property (nonatomic) BOOL eventsPerRequest;

/**
 Manually start a dispatch of all pending events.
 
 @return YES if the dispatch process was started.
 */
- (BOOL)dispatch;

/**
 Delete all pending events.
 */
- (void)deleteQueuedEvents;


/**
 @name Custom visit variables
 */

/**
 The application name.
 
 The application name will be sent as a custom variable (index 2). By default the application name stored in CFBundleDisplayName will be used.
 */
@property (nonatomic, strong) NSString *appName;

/**
 The application version.
 
 The application version will be send as a custom variable (index 3). By default the application version stored in CFBundleVersion will be used.
 */
@property (nonatomic, strong) NSString *appVersion;

@end
