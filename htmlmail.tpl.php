<?php

/**
 * @file
 * Default template for HTML Mail
 *
 * DO NOT EDIT THIS FILE. Copy it to your theme directory, and edit the copy.
 *
 * ========================================================= Begin instructions.
 *
 * When formatting an email message with a given $module and $key, [1]HTML
 * Mail will use the first template file it finds from the following list:
 *  1. htmlmail--$module--$key.tpl.php
 *  2. htmlmail--$module.tpl.php
 *  3. htmlmail.tpl.php
 *
 * For each filename, [2]HTML Mail looks first in the chosen Email theme
 * directory, then in its own module directory, before proceeding to the
 * next filename.
 *
 * For example, if example_module sends mail with:
 * drupal_mail("example_module", "outgoing_message" ...)
 *
 *
 * the possible template file names would be:
 *  1. htmlmail--example_module--outgoing_message.tpl.php
 *  2. htmlmail--example_module.tpl.php
 *  3. htmlmail.tpl.php
 *
 * For another example, to customize the [3]password reset messages sent
 * by the [4]user module, copy [5]htmlmail.tpl.php to a file called
 * htmlmail--user--password_reset.tpl.php in your Email theme directory,
 * and edit the copy.
 *
 * Template files are cached, so remember to clear the cache by visiting
 * admin/config/development/performance after changing any .tpl.php files.
 *
 * The following variables available in this template:
 *
 * $body
 *        The message body text.
 *
 * $module
 *        The sending module name, usually the first parameter to
 *        [6]drupal_mail().
 *
 * $key
 *        The message key, usually the second parameter to
 *        [7]drupal_mail().
 *
 * $message_id
 *        The email message id, usually "{$module}_{$key}".
 *
 * $theme
 *        The name of the Email theme used to hold template files. If the
 *        [8]Echo module is enabled this theme will also be used to
 *        transform the message body into a fully-themed webpage.
 *
 * $template_path
 *        The relative path to the template directory.
 *
 * $template_url
 *        The absolute URL to the template directory.
 *
 * $debug
 *        TRUE to add some useful debugging info to the bottom of the
 *        message.
 *
 * The module calling [9]drupal_mail() may set other variables. For
 * instance, the [10]Webform module sets a $node variable which may be
 * very useful.
 *
 * Other modules may also add or modify theme variables by implementing a
 * MODULENAME_preprocess_htmlmail(&$variables) [11]hook function.
 *
 * References
 *
 * 1. http://drupal.org/project/htmlmail
 * 2. http://drupal.org/project/htmlmail
 * 3. http://api.drupal.org/api/drupal/modules--user--user.pages.inc/function/user_pass_submit/7
 * 4. http://api.drupal.org/api/drupal/modules--user--user.module/7
 * 5. http://drupalcode.org/project/htmlmail.git/blob/refs/heads/7.x-2.x:/htmlmail.tpl.php
 * 6. http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7
 * 7. http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7
 * 8. http://drupal.org/project/echo
 * 9. http://api.drupal.org/api/drupal/includes--mail.inc/function/drupal_mail/7
 * 10. http://drupal.org/project/webform
 * 11. http://api.drupal.org/api/drupal/modules--system--theme.api.php/function/hook_preprocess_HOOK/7
 *
 * =========================================================== End instructions.
 */
?>
<div class="htmlmail-body">
<?php echo $body; ?>
</div>
<?php if ($debug):
  $module_template = "htmlmail--$module";
  $message_template = "$module--$key"; ?>
<hr />
<div class="htmlmail-debug">
  <dl><dt><p>
    To customize this message:
  </p></dt><dd><ol><li><p>
    Copy the
    <a href="http://drupalcode.org/project/htmlmail.git/blob/refs/heads/7.x-2.x:/htmlmail.tpl.php"><code>html.tpl.php</code></a>
    file to the
    <u><code><?php echo realpath(drupal_get_path('theme', $theme)); ?></code></u>
    directory of your <u><?php echo ucfirst($theme) ?></u> theme.
  </p></li><li><p>
    Make two more copies in the same directory, called
    <code><?php echo $module_template; ?>.tpl.php</code> and
    <code><?php echo $message_template; ?>.tpl.php</code>.
  </p></li><li><p>
    For module-specific customization, edit the
    <code><?php echo $module_template; ?>.tpl.php</code> and
    file.
  </p><p>
    For message-specific customization, edit the
    <code><?php echo $message_template; ?>.tpl.php</code>.
    file.
  </p></li></ol></dd></dl>
</div>
<?php endif; ?>
<?php
