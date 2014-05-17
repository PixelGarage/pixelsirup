<?php
/**
 * @file
 * Default view template to display filter buttons for a magical Isotope layout.
 */
?>
<div class="ui-group">
  <?php if ($title): ?>
    <h3 class="group-title"><?php print $title; ?></h3>
  <?php endif; ?>
  <div id="<?php print $button_group_id; ?>" class="filters button-group clearfix">
    <?php if ($reset_label): ?>
      <button class="reset selected" data-filter="*"><?php print $reset_label; ?></button>
    <?php endif; ?>
    <?php foreach ($rows as $id => $row): ?>
      <button class="<?php print $filter_array[$id]; ?>" data-filter=".<?php print $filter_array[$id]; ?>"><?php print $row; ?></button>
    <?php endforeach; ?>
  </div>
</div>

