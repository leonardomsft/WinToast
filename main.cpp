#include "wintoastlib.h"
#include <string>

using namespace WinToastLib;

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"Toast activated: The user clicked in this toast" << std::endl;
        exit(0);
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"Toast activated: The user clicked on action #" << actionIndex << std::endl;
        exit(16 + actionIndex);
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            std::wcout << L"Toast was dismissed by the user" << std::endl;
            exit(1);
            break;
        case TimedOut:
            std::wcout << L"Toast timed out (was not clicked)" << std::endl;
            exit(2);
            break;
        case ApplicationHidden:
            std::wcout << L"Toast was hidden by calling ToastNotifier.hide()" << std::endl;
            exit(3);
            break;
        default:
            std::wcout << L"Toast was not activated (not clicked)" << std::endl;
            exit(4);
            break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing toast" << std::endl;
        exit(5);
    }
};


enum Results {
	ToastClicked,					// user clicked on the toast
	ToastDismissed,					// user dismissed the toast
	ToastTimeOut,					// toast timed out
	ToastHided,						// application hid the toast
	ToastNotActivated,				// toast was not activated
	ToastFailed,					// toast failed
	SystemNotSupported,				// system does not support toasts
	UnhandledOption,				// unhandled option
	MultipleTextNotSupported,		// multiple texts were provided
	InitializationFailure,			// toast notification manager initialization failure
	ToastNotLaunched				// toast could not be launched
};


#define COMMAND_TEXT		L"--text"
#define COMMAND_ATTRIBUTE   L"--attribute"
#define COMMAND_ACTION		L"--action"
#define COMMAND_AUMI		L"--aumi"
#define COMMAND_APPNAME		L"--appname"
#define COMMAND_APPID		L"--appid"
#define COMMAND_EXPIRES	    L"--expires"
#define COMMAND_HELP		L"--help"
#define COMMAND_IMAGE		L"--image"
#define COMMAND_SHORTCUT	L"--only-create-shortcut"
#define COMMAND_AUDIOSTATE  L"--audio-state"

void print_help() {
	std::wcout << "\n WinToast - Toast Notification Generator \n https://github.com/leonardomsft/WinToast \n" << std::endl;
    std::wcout << "  Usage: WinToast.exe [SWITCHES]" << std::endl;
    std::wcout << "\t" << COMMAND_TEXT << L"\t\t(optional) : sets the text for the notifications" << std::endl;
    std::wcout << "\t" << COMMAND_ATTRIBUTE << L"\t(optional) : sets the attribute for the notification" << std::endl;
    std::wcout << "\t" << COMMAND_ACTION << L"\t(optional) : sets the actions in buttons" << std::endl;
	std::wcout << "\t" << COMMAND_AUMI << L"\t\t(optional) : sets the App User Model Id" << std::endl;
	std::wcout << "\t" << COMMAND_APPNAME << L"\t(optional) : sets the default appname" << std::endl;
	std::wcout << "\t" << COMMAND_APPID << L"\t\t(optional) : sets the App User Model Id (AUMI)" << std::endl;
	std::wcout << "\t" << COMMAND_EXPIRES << L"\t(optional) : sets the expiration time in seconds" << std::endl;
	std::wcout << "\t" << COMMAND_IMAGE << L"\t\t(optional) : sets the image absolute path" << std::endl;
    std::wcout << "\t" << COMMAND_SHORTCUT << L"\t(optional) : creates the shortcut for the app" << std::endl;
    std::wcout << "\t" << COMMAND_AUDIOSTATE << L"\t(optional) : sets the audio state: Default = 0, Silent = 1, Loop = 2" << std::endl;
    std::wcout << "\t" << COMMAND_HELP << L"\t\t(optional) : prints this help" << std::endl << std::endl;
    std::wcout << "  Examples: " << std::endl;
    std::wcout << "\t WinToast.exe --text \"Pizza tonight?\" --attribute \"Napoletana\" --action Yes --action No" << std::endl;
    std::wcout << "\t WinToast.exe --image \"C:\\Temp\\189122.png\"" << std::endl;
    std::wcout << "\n" << std::endl;
}


int wmain(int argc, LPWSTR *argv)
{
    if (argc == 1) {
        print_help();
    }

    if (!WinToast::isCompatible()) {
        std::wcerr << L"Error, your system in not supported!" << std::endl;
        return Results::SystemNotSupported;
    }

    LPWSTR text = NULL;
    LPWSTR attribute = NULL;
    LPWSTR imagePath = NULL;
    LPWSTR appName = NULL;
    LPWSTR appUserModelID = NULL;
    std::vector<std::wstring> actions;
    INT64 expiration = 0;
    bool onlyCreateShortcut = false;
    WinToastTemplate::AudioOption audioOption = WinToastTemplate::Default;

    int i;
	for (i = 1; i < argc; i++)
        if (!wcscmp(COMMAND_IMAGE, argv[i]))
            imagePath = argv[++i];
        else if (!wcscmp(COMMAND_ACTION, argv[i]))
            actions.push_back(argv[++i]);
        else if (!wcscmp(COMMAND_EXPIRES, argv[i]))
            expiration = wcstol(argv[++i], NULL, 10);
        else if (!wcscmp(COMMAND_APPNAME, argv[i]))
            appName = argv[++i];
        else if (!wcscmp(COMMAND_AUMI, argv[i]) || !wcscmp(COMMAND_APPID, argv[i]))
            appUserModelID = argv[++i];
		else if (!wcscmp(COMMAND_TEXT, argv[i]))
			text = argv[++i];
        else if (!wcscmp(COMMAND_ATTRIBUTE, argv[i]))
            attribute = argv[++i];
		else if (!wcscmp(COMMAND_SHORTCUT, argv[i]))
			onlyCreateShortcut = true;
        else if (!wcscmp(COMMAND_AUDIOSTATE, argv[i]))
            audioOption = static_cast<WinToastTemplate::AudioOption>(std::stoi(argv[++i]));
		else if (!wcscmp(COMMAND_HELP, argv[i])) {
			print_help();
			return 0;
		} else {
            std::wcerr << L"Option not recognized: " << argv[i]  << std::endl;
			return Results::UnhandledOption;
        }

    if (onlyCreateShortcut) {
        if (imagePath || text || actions.size() > 0 || expiration) {
            std::wcerr << L"--only-create-shortcut does not accept images/text/actions/expiration" << std::endl;
            return 9;
        }
        enum WinToast::ShortcutResult result = WinToast::instance()->createShortcut();
        return result ? 16 + result : 0;
    }

    if (!text) {
        text = L"Very Important Reminder";
        std::wcout << L"Text not specified, using: " << text << std::endl;
    }
    if (!attribute) {
        attribute = L"Don't worry, be happy!";
        std::wcout << L"Attribute not specified, using: "<< attribute << std::endl;
    }
    if (!appName) {
        appName = L"WinToast";
        std::wcout << L"AppName not specified, using: "<< appName << std::endl;
    }
    if (!appUserModelID) {
        appUserModelID = L"WinToast.ID";
        std::wcout << L"App User Model ID (AUMI) not specified, using: " << appUserModelID << std::endl;
    }
    if (expiration == 0) {
        expiration = 60;
        std::wcout << L"Expiration not specified, using: " << expiration << L" seconds" << std::endl;
    }
    else {
        expiration = expiration * 1000;
    }
    if (!imagePath) {
        std::wcout << L"Image not specified, using no image." << std::endl;
    }


    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(appUserModelID);


    if (!WinToast::instance()->initialize()) {
        std::wcerr << L"Error, your system in not compatible!" << std::endl;
        return Results::InitializationFailure;
    }

    bool withImage = (imagePath != NULL);
	WinToastTemplate templ( withImage ? WinToastTemplate::ImageAndText02 : WinToastTemplate::Text02);
	templ.setTextField(text, WinToastTemplate::FirstLine);
    templ.setAudioOption(audioOption);
    templ.setAttributionText(attribute);

	for (auto const &action : actions)
        templ.addAction(action);
    if (expiration)
        templ.setExpiration(expiration);
    if (withImage)
        templ.setImagePath(imagePath);


    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0)
    {
        std::wcerr << L"Could not launch your toast notification!";
        return Results::ToastFailed;
    }
    else 
    {
        std::wcout << L"Toast notification successfully sent!" << std::endl;
    }

    // Sleeping for 10 seconds, while the handler either activate/dismiss
    Sleep(10000);

    exit(2);
}
