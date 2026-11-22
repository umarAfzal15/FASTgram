# FASTgram: C++17 & Raylib Campus Social Platform

FASTgram is a desktop-focused campus social media platform engineered in modern C++17 using Raylib for graphics and user interface rendering. Developed specifically for the FAST-NUCES academic ecosystem, the application combines traditional social networking features (such as photo sharing, threaded comments, and real-time messaging) with university-specific tools like roll number verification, notice boards, society management, and engagement badges.

## 1. Project Overview & Technical Architecture

The application is built around a centralized 480x820 pixel application frame designed to simulate a desktop mobile experience. The UI container supports dynamic window resizing down to a minimum resolution of 360x640 pixels while keeping the central canvas aligned.

The project strictly follows a 5-layer decoupled architecture to ensure clear separation of concerns, testability, and zero circular dependencies:

1. Core Layer (`src/core/`): Fundamental utility classes, theme management, custom time formatting, logger components, and exception classes.

2. Models Layer (`src/models/`): Domain entities including Account types, Post types, Comments, Messages, Notifications, and Badges.

3. Services Layer (`src/services/`): Business logic managers (`PostManager`, `ChatManager`, `SocialGraph`, `PostFactory`) that process interactions and manage object lifecycles.

4. UI Widgets (`src/ui/widgets/`): Reusable graphical components such as `Button`, `TextBox`, `ScrollView`, `Avatar`, `Toast`, and `Modal`.

5. UI Screens (`src/ui/screens/`): Top-level screen views managed by `ScreenManager`, such as `LoginScreen`, `SignUpScreen`, `MainShellScreen`, and `SettingsScreen`.

## 2. Implemented Features & Core Functional Modules

### Accounts & Authentication

* Account Hierarchy: Abstract base `Account` class with derived classes for `Student`, `Faculty`, `Society`, and `Admin`. Each class enforces specific permission boundaries (for example, notice posting reserved for Faculty, Admin, and Society accounts).

* Roll Number Validation: Enforces strict format checks (e.g., `i26-1234`) on registration for student accounts.

* Session Control: Centralized `Session` manager tracking logged-in user state without relying on global variables.

* Authentication UI: Dedicated `LoginScreen` and `SignUpScreen` with real-time text input validation and error feedback.

### Desktop UI Engine & Custom Widgets

* Widget Base Framework: Unified `Widget` abstract class implementing hover state detection, text dynamic rendering, custom cursor switching, and tooltip overlays.

* ScrollView & Avatar: Custom scissor-clipping container (`ScrollView`) supporting smooth mouse-wheel scrolling and edge clamping, paired with a circular avatar component featuring fallback initials and unread story rings.

* Theme Engine: Centralized runtime `Theme` engine allowing instant switching between Light Mode and Dark Mode without restarting the program.

* Screen Management: Stack-based `ScreenManager` maintaining the 5-tab main application shell (`MainShellScreen`) while supporting push and pop transitions for detail views.

### Persistence & File Handling

* Binary Serialization: Custom binary serialization system using `Serializable` interfaces. Data is written with an `FTG1` magic header, version tagging, count metadata, and length-prefixed strings. File updates use atomic temp-file swapping to prevent data corruption.

* Startup Validation: `FileCorruptException` handling during application launch to gracefully handle missing or altered data files.

* Config & Activity Logging: Reads and writes runtime configuration (theme preference, window size) to `data/config.txt`, while appending timestamped user actions to `data/activity.log`.

### Communication & Gamification

* Notification System: Polymorphic notification management (`NotificationManager`) supporting `LikeNotification`, `FollowNotification`, and `CommentNotification` with read status indicators.

* Badge & Streak Tracking: Base `Badge` system evaluating account activity for achievements such as `FirstPostBadge` and daily streak tracking.

## 3. Directory & File Structure

```
FASTgram/
├── data/                       # Storage for binary dat files, indices, logs, and configs
│   ├── activity.log            # Human-readable append-only event log
│   └── config.txt              # Application startup configuration
├── resources/                  # Visual assets and media
│   ├── fonts/                  # TrueType fonts used across the application
│   ├── icons/                  # Interface icons in PNG format
│   └── images/                 # Image storage for avatars and post media
│       ├── avatars/            # User profile image assets
│       └── posts/              # Uploaded post image assets
├── src/
│   ├── app/                    # Main loop and Application initialization class
│   ├── core/                   # Utilities: Theme, Logger, Time, Exceptions, Session
│   ├── models/                 # Domain objects: Account, Post, Comment, Message, Badge
│   ├── services/               # Logic engines: PostManager, ChatManager, SocialGraph
│   └── ui/                     # Graphical interface layer
│       ├── screens/            # Screen views: Login, MainShell, Settings
│       └── widgets/            # Components: Button, ScrollView, Modal, Toast
├── FASTgram.sln                # Visual Studio 2022 Solution File
├── README.md                   # Project Overview & Manual
└── CREDITS.md                  # Asset sources and copyright declarations

```

## 4. Building & Running the Project

### Prerequisites

* Windows 10 or Windows 11

* Visual Studio 2022 (with "Desktop development with C++" workload installed)

* ISO C++17 Standard compiler flag (`/std:c++17`)

* Raylib library files linked to the solution

### Compilation Steps

1. Clone the repository to your local directory:
   `git clone https://github.com/umarAfzal15/FASTgram.git`

2. Launch Visual Studio 2022 and open `FASTgram.sln`.

3. Verify that the project property settings point to your local Raylib installation:

   * Additional Include Directories: Pointing to `raylib/include`

   * Additional Library Directories: Pointing to `raylib/lib`

   * Additional Dependencies: `raylib.lib;winmm.lib;gdi32.lib;shell32.lib`

4. Set the build target to `Debug` or `Release` under the `x64` architecture.

5. Press `F5` to compile and run the application.

## 5. Verification & Testing Workflow

To ensure system stability, the following testing procedures are carried out:

1. Data Persistence Verification:

   * Perform user registration and login, then close the application.

   * Inspect `data/config.txt` and `data/activity.log` to confirm correct state updates.

   * Relaunch the application to verify automatic session restoration and index loading.

2. Visual & Theme Stress Testing:

   * Open `SettingsScreen` and toggle between Light and Dark modes repeatedly to ensure all container colors update without visual artifacts or memory leaks.

   * Resize the application window below standard dimensions to verify content clamping within `ScrollView` containers.

3. Serialization Integrity Checks:

   * Pass corrupted inputs or empty files into the data folder to verify that `FileCorruptException` traps errors without crashing the main process loop.

## 6. Architecture & Memory Rules

* Ownership Model: Single ownership rule enforced. Managers (`PostManager`, `ChatManager`, `ScreenManager`) own heap-allocated object pointers (`Post*`, `Message*`, `Screen*`) and execute cleanup in their respective destructors.

* Header Inclusion Constraint: Lower layers (`models`, `core`) must never include headers from higher layers (`ui/widgets`, `ui/screens`). All UI modules depend on core models, but core models remain completely decoupled from Raylib and UI code.