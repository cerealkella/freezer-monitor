/*
Modify as necessary and rename this file to:
email_settings.h
*/

#define email_server "mail.smtp2go.com"
#define email_port 2525

// change to your base64 encoded username
#define email_user "Base64EncodedUsername"
// change to your base64 encoded password
#define email_password "Base64EncodedPASSWORD"

#define email_from "<alert@your_domain.com>"
#define email_to "youremail@whatever.com"

/*
 * Use the following variables in order to set a static IP
  (add this to the freezer-monitor.ino file)
  IPAddress ip( 192, 168, 1, 20 );
  IPAddress gateway( 192, 168, 1, 1 );
  IPAddress subnet( 255, 255, 255, 0 );
  IPAddress dns( 8, 8, 8, 8 );

  Then call 
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  otherwise for DHCP just use
  Ethernet.begin(mac)
 */
