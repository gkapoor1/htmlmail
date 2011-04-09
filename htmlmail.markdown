When formatting an email message with a given `$module` and `$key`,
[HTML Mail](http://drupal.org/project/htmlmail)
will use the first template file it finds from the following list:

1. `htmlmail--$module--$key.tpl.php`
2. `htmlmail--$module.tpl.php`
3. `htmlmail.tpl.php`

For each filename,
[HTML Mail](http://drupal.org/project/htmlmail)
looks first in the chosen *Email theme* directory, then in its own
module directory, before proceeding to the next filename.

For example, if `example_module` sends mail with:

    drupal_mail("example_module", "outgoing_message" ...)

the possible template file names would be:

*    `htmlmail--example_module--outgoing_message.tpl.php`
*    `htmlmail--example_module.tpl.php`
*    `htmlmail.tpl.php`

For another example, to customize the
[password reset](http://api.drupal.org/api/drupal/modules--user--user.pages.inc/function/user_pass_submit/7)
messages sent by the
[user module](http://api.drupal.org/api/drupal/modules--user--user.module/7),
copy
[`htmlmail.tpl.php`](http://drupalcode.org/project/htmlmail.git/blob/refs/heads/7.x-2.x:/htmlmail.tpl.php)
to a file called
`htmlmail--user--password_reset.tpl.php`
in your *Email theme* directory, and edit the copy.

Template files are cached, so remember to clear the cache by visiting
<u>admin/config/development/performance</u>
after changing any `.tpl.php` files.

The following variables available in this template:

**`$body`**
:   The message body text.

**`$module`**
:   The sending module name, usually the first parameter to
[`drupal_mail()`](http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7).

**`$key`**
:   The message key, usually the second parameter to
[`drupal_mail()`](http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7).

**`$message_id`**
:   The email message id, usually `"{$module}_{$key}"`.

**`$theme`**
:   The name of the email-specific theme used to embed the message body into a
    fully-themed webpage.

    **Note:** This may be different from the default website theme.  Theme
    suggestion templates such as `html.tpl.php` should be copied to the
    *website theme directory*, not the *email theme directory*.

**`$directory`**
:   The relative path to the website theme template directory.

**`$theme_url`**
:   The absolute URL to the website theme directory.

**`$debug`**
:   `TRUE` if debugging info should be printed.

The module calling
[`drupal_mail()`](http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7)
may set other variables.  For instance, the
[Webform module](http://drupal.org/project/webform)
sets a `$node` variable which may be very useful.

Other modules may also add or modify theme variables by implementing a
`MODULENAME_preprocess_htmlmail(&$variables)`
[hook function](http://api.drupal.org/api/drupal/modules--system--theme.api.php/function/hook_preprocess_HOOK/7).
