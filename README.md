# Status Logger

_⚡ a Sparkmate Let's Build brick (lib) ⚡_

A more advanced status logger that makes logging and status reporting easier. Includes features:

-   Log to Serial and to a recallable buffer
-   Report the status of various bricks within your code (your sensors, actuators, components, etc.)
-   Recall important logs and statusses at a later time
-   Pipe logs and statuses to other Streams, e.g. a file on the SD card

## Usage

```cpp
    /** Print a brick status into a Serial readible format. */
    String deconstructBrickStatus(BRICK_STATUS brick_status);

    /** Log something to the StatusLogger system
     If the (log) level is less than LOG_LEVEL_SAVE then we'll print both to Serial and to the cacher to be retrieved later (use sparingly)! */
    void log(LOG_LEVEL_td level, String device, String msg, bool print_to_serial = false, Stream *stream = &Serial);

    /** Retrieve logs previously cached (good for log dumps of important logs as opposed to just Serial.print). -> Use this to dump logs to an SD card with an opened file as your stream. */
    void retrieveLogs(Stream *stream = &Serial);

    /** Return the index for the status of a given brick.
    Returns -1 if the brick is not found, otherwise the index of the brick's status. */
    int getBrickStatusIndex(String device)

    /**  Get the Brick Status object at a given index. */
    BRICK_STATUS getBrickStatus(int i)

    /** Evaluate if a given device has the same functionality status you're checking against.
    Returns 1 if the device at the given index has the same status, 0 if the device exists at the given index but the status is not the same, and -1 if the device does not exist at the given index. */
    int evalBrickStatus(String device, LOG_FUNCTIONALITY_td status)

    /** Update a brick's status (for example if internet failed), options of: [FUNCTIONAL, PARTIAL, OFFLINE]. Automatically makes a log status update. */
    void setBrickStatus(String device, LOG_FUNCTIONALITY_td functionality, String msg = "");

    /** Retrieve all of the latest brick statuses and print to logs (default is Serial) */
    void printBrickStatuses(Stream *stream = &Serial);

    /** Set the session details at the beginning of operation. Set once and forget! */
    void setSessionDetails(String device_ID, String session_filename, time_t session_start_time);

    /** Print only the session details to Serial. */
    void printSessionDetails();

```

# The Sparkmate Let's Build open source policy

This boilerplate is open source, reusable, and hackable by design. **We want you to build** with it. We don't ask for credit, attribution, funding, or anything like that.
We just want you to make something cool, and we hope we've helped 😉.

Please add any issues or MRs as you see fit.

Get in touch with [letsbuild@sparkmate.co](mailto:letsbuild@sparkmate.co) for any questions or comments.
