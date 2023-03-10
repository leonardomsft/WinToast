# WinToast - Toast Notification Generator

Utility to generate and send local toast notifications on Windows 10. Toast notifications can include with text, attributes, images, and action buttons.

# Command line switches
```
  --text : Set the text for the notification
  
  --attribute : set the attribute for the notification
  
  --image : set the image path
  
  --action : Set the actions in buttons
  
  --aumi : Set the App User Model Id
  
  --appname : Set the default appname
  
  --appid : Set the App Id
  
  --expirems : Set the default expiration time
  
  --only-create-shortcut : create the shortcut for the app
  
  --audio-state : set the audio state: Default = 0, Silent = 1, Loop = 2
  
  --help : Print the help description
```

# Examples

WinToast.exe --text "My Toast Notification" --attribute "My Attributes" --appname "WinToast" --aumi "WinToast" --expirems 10000 --action Yes --action No --image "C:\Tools\189122.png"


# More on Toast Notifications

## App User Model Id (AUMI)
The App sending a toast notification must have a shell shortcut with the App User Model ID peroperty specified, otherwise the notification may fail or not display properly. Wintoast automatically creates and maintains this shortcut at the APPDATA location.
  
## Expiration vs Duration
Expiration defines how long the toast notification will remain in the Action Center. Duration is the time it will be visible on the user's screen.
Before expiration, the notification can still be seen in the Action Center (provided that the user hasn't dismissed it). After expiration, it's deleted and completely disappears.
Duration depends on the template used and can be either "short" or "long". Wintoast uses the "ToastGeneric" template and defines "short" notifications.

# Resources

Send a local toast notification from a WRL C++ desktop app

  https://learn.microsoft.com/en-us/windows/apps/design/shell/tiles-and-notifications/send-local-toast-desktop-cpp-wrl


How to enable desktop toast notifications through an AppUserModelID

https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/hh802762(v=vs.85)

