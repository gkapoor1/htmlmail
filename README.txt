HTML Mail
---------

HTML Mail empowers Drupal with the ability to send emails in HTML, providing
formatting and semantic markup capabilities in email that are not available
with plain text.

For a full description of the module, visit the project page:
  http://drupal.org/project/htmlmail

To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/htmlmail

Installation
------------

Install as usual; see http://drupal.org/node/895232 for further information.

Theming
-------

The default "Theme to use for emails" is "No theme (use htmlmail.tpl.php)".
To customize the appearance of your outgoing emails, you should copy the
included "htmlmail.tpl.php" file to your theme directory and edit the
contents.

You may also create "htmlmail-MODULENAME.tpl.php" files to customize mail sent by
a particular module.  For instance, to customize the password reset emails sent
by the user module, copy "htmlmail.tpl.php" to "htmlmail-user.tpl.php".  Remember
that you will need to put both files in your theme directory for this to work.

Template files are cached, so remember to clear cache (admin/settings/performance)
after creating, copying, or editing any .tpl.php files.

If you prefer your emails to look just like your website pages, choose an
available theme from the "Theme to use for emails" selection box.

The Emogrifier library (http://www.pelagodesign.com/sidecar/emogrifier/) may be
used to convert stylesheets to inline style rules.  A copy is provided with this
module.  If you would rather maintain a separately-updated copy, install the
libraries module (http://drupal.org/project/libraries).

For tips and resources on building HTML emails see:
* http://www.campaignmonitor.com/css/
* http://www.mailchimp.com/articles/email_marketing_guide/
* http://css-tricks.com/using-css-in-html-emails-the-real-story/

Important
---------

Remember that many email clients will not be happy with certain code.  Your
CSS may conflict with a web-mail provider's CSS, and HTML in email may expose
security vulnerabilities. Beyond this, if you still really, really must have
HTML in your email, you may find this module useful.


Maintainers
-----------

Chris Herberte - http://drupal.org/user/1171
Bob Vincent - http://drupal.org/user/36148
