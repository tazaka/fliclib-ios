//
//  @file SCLFlicManager.h
//  @framework fliclib
//
//  Created by Anton Meier on 2014-06-18.
//  Copyright (c) 2016 Shortcut Labs. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "SCLFlicButton.h"

/*!
 *  @enum SCLFlicManagerBluetoothState
 *
 *  @discussion These enums represents the different possible states that the manager can be in at any given time.
 *              The manager needs to be in the <code>SCLFlicManagerBluetoothStatePoweredOn</code> state in order for it to perform
 *              any kind of communication with a flic.
 *
 */
typedef NS_ENUM(NSInteger, SCLFlicManagerBluetoothState) {
    /**
     * This state is the desired state that is needed when communicating with a flic.
     */
    SCLFlicManagerBluetoothStatePoweredOn = 0,
    /**
     * This state means that the manager is currently powered off and will not be able to perform any bluetooth related tasks.
     * This will for example be the case when bluetooth is turned off on the iOS device.
     */
    SCLFlicManagerBluetoothStatePoweredOff,
    /**
     * The manager is resetting and will most likely switch to the powered on state shortly.
     */
    SCLFlicManagerBluetoothStateResetting,
    /**
     * The manager was not able to turn on because the iOS device that it is currently running on does not support Bluetooth Low Energy.
     */
    SCLFlicManagerBluetoothStateUnsupported,
    /**
     * The manager was not able to turn on because the app is not authorized to to use Bluetooth Low Energy.
     */
    SCLFlicManagerBluetoothStateUnauthorized,
    /**
     * The manager is in an unknown state, it will most likely change shortly.
     */
    SCLFlicManagerBluetoothStateUnknown,
};

@protocol SCLFlicManagerDelegate;

/*!
 *  @class SCLFlicManager
 *
 *  @discussion     An instance of this class is required in order to perform any bluetooth LE communication with
 *                  the flic. The object will keep track of all the flics that are associated to the specific iOS device.
 *                  It is important to mention that the SCLFlicManager does not support the regular state preservation
 *                  and restoration protocol for view controllers, meaning that it does not support NSCoding.
 *                  Instead, all of the state preservation will be taken cared of for you by the manager.
 *                  Simply reinstantiate it using the <code>initWithDelegate:appID:appSecret:andRestoreState:</code> method
 *                  and collect the associated flic objects using the <code>knownButtons:</code> method.
 *
 */
@interface SCLFlicManager : NSObject {
    
}

/*!
 *  @property delegate
 *
 *  @discussion     The delegate object that will receive all the flic related events. See the definition of the
 *                  SCLFlicManagerDelegate to see what callbacks are available.
 *
 */
@property(weak, nonatomic, nullable) id<SCLFlicManagerDelegate> delegate;

/*!
 *  @property bluetoothState
 *
 *  @discussion     The current bluetoothState of the flic manager. A bluetoothDidChangeState event will be generated
 *                  on the SCLFlicManagerDelegate whenever this state has changed. When the flic manager is initialized
 *                  the state will be SCLFlicManagerBluetoothStateUnknown by default. You will not be able to do any
 *                  bluetooth related tasks until the manager properly changes to SCLFlicManagerBluetoothStateOn.
 *
 */
@property (nonatomic, readonly) SCLFlicManagerBluetoothState bluetoothState;

/*!
 *  @property enabled
 *
 *  @discussion     This is a flag that lets you know if the manager is enabled for Bluetooth LE communication. This can be toggled on/off using the
 *                  <code>enable</code> and <code>disable</methods>. When This property is set to <code>NO</code>, then no Bluetooth LE communication
 *                  will be allowed. This means that no communication with a flic can be made.
 */
@property (readonly, getter=isEnabled) BOOL enabled;

/*!
 *  @method initWithDelegate:appID:appSecret:backgroundExecution:andRestoreState:
 *
 *  @discussion     The initialization call to use when you want to create a manager. This will initiate a SCLFlicManager and do the proper
 *                  preparation needed in order to start the bluetooth communication with flic buttons. The BOOL <code>restore</code> included
 *                  in the initialization call decides whether a brand new manager will be created or if it should be created and restored to
 *                  the last known state of the most previous manager created by the same app. If you choose to create a new manager then any old
 *                  manager (if existing) will be irreversibly cleared. Using more than one manager in the same application is not supported.
 *                  <br/><br/>
 *                  When choosing the restore option all settings on the manager will be restored. This will also recreate all SCLFlicButtons
 *                  that had previously been used with this manager unless they had been manually removed using the <code>forgetButton:</code>
 *                  method. All the flic objects that had a pending connection before will be set to the same state after restoration. When the
 *                  restoration process is complete the manager will call the <code>flicManagerDidRestoreState:</code> method of its delegate
 *                  object. At that point it is recommended that you call <code>knownButtons:</code> in order to collect all the flic objects and
 *                  re-set their delegate.
 *
 *  @param delegate     The delegate that all callbacks will be sent to.
 *  @param appID        This is the App-ID string that the developer is required to have in order to use the fliclib framework.
 *  @param appSecret    This is the App-Secret string that the developer is required to have in order to use the fliclib framework.
 *  @param bExecution   Flag that specifies if want to configure the Flic manager to support background execution. If YES, then you also
 *                      have to check the "Uses Bluetooth LE accessories" option under your projects Background Modes settings.
 *  @param restore      Whether you want to create a brand new manager (and thus clearing any old manager) or restore the manager to a
 *                      previous state.
 *
 */
- (instancetype _Nullable) initWithDelegate:(id<SCLFlicManagerDelegate> _Nonnull) delegate appID: (NSString * _Nonnull) appID appSecret: (NSString * _Nonnull) appSecret backgroundExecution: (BOOL) bExecution andRestoreState:(BOOL) restore;

/*!
 *  @method knownButtons:
 *
 *  @discussion     All buttons that have ever been added to the manager and not manually been forgotten/removed.
 *
 *  @return         Dictionary containing the SCLFlicButton objects. The keys are of NSUUID type that represent the buttonIdentifier
 *                  of the SCLFlicButton instance.
 *
 */
- (NSDictionary<NSUUID *, SCLFlicButton *> * _Nonnull) knownButtons;

/*!
 *  @method forgetButton:
 *
 *  @discussion     This will attempt to completely remove the flic button from the manager and clear the SCLFlicButton instance. If the flic
 *                  is connected when this method is called then it will also be disconnected first. Remember to clear all your references
 *                  to this particular button instance so that it properly gets cleared from memory.
 *
 *  @param button   The button that you wish to destroy.
 *
 */
- (void) forgetButton:(SCLFlicButton * _Nonnull) button;

/*!
 *  @method disable
 *
 *  @discussion     This will disable all bluetooth communication and disconnect all currently connected buttons and pending connections.
 *                  You will not be able to do any communication with a flic until you call <code>enable</enable>.
 *
 */
- (void) disable;

/*!
 *  @method enable
 *
 *  @discussion     This will enable the bluetooth communication after it has previously been disabled. It will not however reconnect any buttons,
 *                  that will have to be handled manually. This is unless you have the flic(s) configured to be in <i>KeepAlive</i> mode, 
 *                  in which case the connect will automatically be sent.
 *
 */
- (void) enable;


/*!
 *  @method requestButtonFromFlicAppWithCallback:
 *
 *  @discussion         This will launch the Flic app and allow the user to select the Flic button to assign to this App.
 *
 *  @param callback     This is the callback URL that you want the Flic app to return the data to.
 *
 */
- (void) requestButtonFromFlicAppWithCallback: (NSString * _Nonnull) callback;

/*!
 *  @method generateButtonFromURL:
 *
 *  @discussion     Use this method in order to generate a Flic button from a URL that was recieved from the flic App callback.
 *
 *  @param url      This is the full url that was returned from the Flic app. 
 *
 */
- (SCLFlicButton * _Nullable) generateButtonFromURL: (NSURL * _Nonnull) url error: (NSError * _Nullable * _Nullable) error;


/*!
 *  @method refreshPendingConnections:
 *
 *  @discussion     This method will go through all pending connections and refresh them. This is needed since we unfortunately can not rely on Apple
 *                  to take care of that. This method should be called at regular intervals, such the once provided by significant location changes.
 *                  Please note that this is ONLY needed if you are using SCLFlicModeBackground!
 *
 */
- (void) refreshPendingConnections;

@end


/*!
 *  @protocol SCLFlicManagerDelegate
 *
 *  @discussion     The delegate of a SCLFlicManager instance must adopt the <code>SCLFlicManagerDelegate</code> protocol. There are no
 *                  required delegate methods, but flicManagerDidChangeBluetoothState is highly recommended.
 *
 */
@protocol SCLFlicManagerDelegate <NSObject>

@required

@optional

/*!
 *  @method flicManager:didChangeBluetoothState:
 *
 *  @param manager      The flic manager providing this information.
 *  @param state        The state that the manager changed to that caused the callback. Notice that there is no guarantee that it has not changed since!
 *
 *  @discussion         If the bluetooth state on the iOS device or the flicManager changes for any reason, then this delegate method will be called
 *                      letting you that something happened. A parameter <code>state</code> will be included, but it is a good practice to always read
 *                      the most current value of the <code>bluetoothState</code> property on the manager to get info about the current state, since
 *                      there is a chance that the state could have changed again while the callback was sent. If the state changes to
 *                      <code>SCLFlicManagerBluetoothStatePoweredOn</code> then all the previous connections and pending connections will be set back to
 *                      pending again.
 *
 */
- (void) flicManager:(SCLFlicManager * _Nonnull) manager didChangeBluetoothState: (SCLFlicManagerBluetoothState) state;

/*!
 *  @method flicManagerDidRestoreState:
 *
 *  @param manager      The flic manager providing this information.
 *
 *  @discussion         This delegate method will be called after the manager has been properly restored after being terminated by
 *                      the system. All the flic buttons that that you had prior to being terminated have been restored as well and
 *                      this is a good time to collect all the SCLFlicButton objects by using the <code>knownButtons:</code> method in
 *                      order to properly restore the rest of your application. Do not forget to re-set the delegate on all buttons.
 *
 */
- (void) flicManagerDidRestoreState:(SCLFlicManager * _Nonnull) manager;

/*!
 *  @method flicManager:didForgetButton:error:
 *
 *  @param manager              The flic manager providing this information.
 *  @param buttonIdentifier     The buttonIdentifier of the SCLFlicButton object that was cleared
 *  @param error                In case there was an error
 *
 *  @discussion         This callback will be made when a flic has been properly forgotten/removed, unless there was an error. Remember
 *                      to also remove your references in case you still have any.
 *
 */
- (void) flicManager:(SCLFlicManager * _Nonnull) manager didForgetButton:(NSUUID * _Nonnull) buttonIdentifier error:(NSError * _Nullable)error;


@end