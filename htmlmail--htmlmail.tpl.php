<?php

/**
 * @file
 * Sample template for HTML Mail test messages.
 */
?>
<h1><a href="http://drupal.org/project/htmlmail">HTML Mail</a> test message</h1>
<div class="htmlmail-body">
<?php print $body; ?>
</div>
<?php if ($debug): ?>
<hr />
<div class="htmlmail-debug">
  To customize this test message:
  <ol>
    <li>Copy the following files to your theme directory:
      <ul>
        <li>
          <a href="http://drupalcode.org/project/htmlmail.git/blob/refs/heads/7.x-2.x:/htmlmail.tpl.php">
            <code>htmlmail.tpl.php</code>
          </a>
        </li>
        <li>
          <a href="http://drupalcode.org/project/htmlmail.git/blob/refs/heads/7.x-2.x:/htmlmail--htmlmail.tpl.php">
            <code>htmlmail--htmlmail.tpl.php</code>
          </a>
        </li>
      </ul>
    </li>
    <li>
      Edit your copy of the <code>htmlmail--htmlmail.tpl.php</code> file.
    </li>
  <ol>
</div>
<?php endif; ?>
<?php
