HTML Mail

   HTML Mail lets you theme your outgoing messages the same way you theme
   the rest of your website.

   For a full description of the module, visit the [1]project page.

   To submit bug reports and feature suggestions, visit the [2]issue
   queue.

Requirements

   HTML Mail depends on the following modules:
     * [3]Mail MIME
     * [4]Mail System
     * [5]Echo

   module.

Installation

   Install as usual; see [6]Installing contributed modules (Drupal 7) for
   further information.

   The following additional modules, while not required, are highly
   recommended:
     * [7]Emogrifier
     * [8]Pathologic
     * [9]Transliteration
       Requires a [10]patch.

Configuration

   Visit the Mail System settings page at admin/config/system/mailsystem
   to select which parts of Drupal will use HTML Mail instead of the
   [11]default [12]mail system.

   Visit the HTML Mail settings page at admin/config/system/htmlmail to
   select a theme, pre-filter, and post-filter for your emails.

Theming

   The email message text goes through four transformations before
   sending:
    1. The Text format pre-filter from the module settings page is
       applied. This should be the same text format that your website uses
       for contributed content such as comments or blog postings. For
       consistency and security, it should include the the [13]Correct
       faulty and chopped off HTML from [14]filter.module, or a better
       replacement such as [15]HTML Purifier or [16]htmLawed.
    2. A theme template is applied. The default template is the included
       htmlmail.tpl.php file. You may copy this file to your theme
       directory and use it to customize the contents and formatting of
       your emails. The comments within the file contain complete
       documentation on its usage.
    3. The message may be wrapped in a website theme selected on the
       module settings page. Creating an email-specific theme lets you use
       the full power of the [17]drupal theme system to format your
       emails.
    4. The Text format post-filter from the module settings page is
       applied. For best results, this should be an email-specific input
       format containing the following text format filters:
          + [18]Transliteration
          + [19]Emogrifier
          + [20]Pathologic
          + [21]Correct faulty and chopped off HTML

Related Modules

   Emogrifier
          http://drupal.org/project/emogrifier

   HTML Mail
          http://drupal.org/project/htmlmail

   HTML Purifier
          http://drupal.org/project/htmlpurifier

   htmLawed
          http://drupal.org/project/htmlawed

   Mail System
          http://drupal.org/project/mailsystem

   Pathologic
          http://drupal.org/project/pathologic

   Transliteration
          http://drupal.org/project/transliteration

Documentation

   filter.module
          http://api.drupal.org/api/drupal/modules--filter--filter.module/
          7

   Installing contributed modules (Drupal 7)
          http://drupal.org/node/895232

   Theming guide
          http://drupal.org/documentation/theme

   Standard filters
          http://api.drupal.org/api/drupal/modules--filter--filter.module/
          group/standard_filters/7

Maintainers

     * [22]Chris Herberte
     * [23]Bob Vincent

References

   1. http://drupal.org/project/htmlmail
   2. http://drupal.org/project/issues/htmlmail
   3. http://drupal.org/project/mailmime
   4. http://drupal.org/project/mailsystem
   5. http://drupal.org/project/echo
   6. http://drupal.org/node/895232
   7. http://drupal.org/project/emogrifier
   8. http://drupal.org/project/pathologic
   9. http://drupal.org/project/transliteration
  10. http://drupal.org/node/1095278#comment-4219530
  11. http://api.drupal.org/api/drupal/modules--system--system.mail.inc/class/DefaultMailSystem/7
  12. http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail_system/7
  13. http://api.drupal.org/api/drupal/modules--filter--filter.module/function/_filter_htmlcorrector/7
  14. http://api.drupal.org/api/drupal/modules--filter--filter.module/7
  15. http://drupal.org/project/htmlpurifier
  16. http://drupal.org/project/htmlawed
  17. http://drupal.org/documentation/theme
  18. http://drupal.org/node/1095278#comment-4219530
  19. http://drupal.org/project/emogrifier
  20. http://drupal.org/project/pathologic
  21. http://api.drupal.org/api/drupal/modules--filter--filter.module/function/_filter_htmlcorrector/7
  22. http://drupal.org/user/1171
  23. http://drupal.org/user/36148
