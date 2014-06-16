<?php
/**
 * @file
 * Default view template to display items in a proximity layout.
 */
?>
<div class="pe-container">
  <div class="pe-background-image"></div>
  <?php foreach ($rows as $id => $row): ?>
    <div class="pe-item <?php print 'pe-item-' . $id; ?> <?php if ($classes_array[$id]) print $classes_array[$id]; ?>">
      <div class="pe-item-inner">
        <?php print $row; ?>
      </div>
    </div>
  <?php endforeach; ?>
</div>
