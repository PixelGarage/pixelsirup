/**
 * ---
 * @author Ralph Moser (http://ramosoft.ch)
 * @version 0.1
 * @updated 10-Apr-2014
 * ---
 */

(function($) {

  /**
   * This behavior adds isotope magical layouts to defined containers and its items. It supports filtering of the items.
   * The behavior is supported in IE 10+ (uses transitions), Chrome, Firefox, Safari and Opera.
   *
   * @type {{attach: Function}}
   */
  Drupal.behaviors.isotopeLayouts =  {
    attach: function() {

      // Iterate through all Isotope instances
      $.each(Drupal.settings.isotope, function (container, settings) {
        // Set container with class 'isotope'
        var $container = $(container).addClass('isotope');


        //
        // Attach filter button events: only available, if items are filterable and filter buttons exist on page
        //
        try {
          // check if filterable items and filter button containers have been defined
          var $filterButtons = Drupal.settings.isotope_filter;
          var hasFilterButtons = (typeof $filterButtons != 'undefined') && Object.keys($filterButtons).length > 0;

          if (settings.filters_enabled && hasFilterButtons) {
            // contains combined filters of each button group
            var groupFilterColl = {};

            // Iterate through all available filter button containers and add button click events
            $.each($filterButtons, function (buttonContainerId, filterSettings) {
                //
                // get button group with given id
                var $button_container = $('#' + buttonContainerId);
                var groupFilter = [],
                    resetButtonSelection = function (container) {
                        container.find('.button.selected').each(function() {
                            var dataFilter = $(this).attr('data-filter'),
                                index = groupFilter.indexOf(dataFilter);
                            if (index >= 0) groupFilter.splice(index,  1); // remove element at index
                            $(this).removeClass('selected');
                        });
                    },
                    updateResetButton = function() {
                        // update reset button selection
                        if ($button_container.find('.button.selected').length > 0) {
                            $button_container.find('.button.reset').removeClass('selected');
                        } else {
                            $button_container.find('.button.reset').addClass('selected');
                        }
                    };

                //
                // attach button events
                $button_container.on('click', '.button', function(ev) {
                    // disable uncover animation for all items (prevent conflicts with filter animation)
                    _disableUncoverAnimation();

                    // create filter value
                    var dataFilter = '',
                        $clickedButton = $(this);

                    // check first, if a parent button on the same level is active, close its child group if open
                    // reset selection on siblings (and their children)
                    var $activeSibling = $clickedButton.siblings('.active-trail');

                    if ($activeSibling.length > 0) {
                        resetButtonSelection($activeSibling);
                        updateResetButton();
                        $activeSibling.removeClass('active-trail');
                        if ($activeSibling.hasClass('visible-children')) {
                            var $siblingGroup = $activeSibling.find('>div.button-group'),
                                containerHeight = $button_container.height(),
                                sgHeight = $siblingGroup.outerHeight(true);
                            $siblingGroup.slideUp(300);
                            $button_container.height(containerHeight - sgHeight);
                            $activeSibling.removeClass('visible-children');
                        }
                    }

                    //
                    // perform button action of clicked button (or one of its parent buttons (bubbling))
                    if ($clickedButton.hasClass('reset')) {
                        //
                        // reset filter and clicked buttons
                        $button_container.find('.button.active-trail').removeClass('active-trail');
                        $button_container.find('.button.selected').removeClass('selected');
                        groupFilter = [];
                        $clickedButton.addClass('selected');

                    } else if ($clickedButton.hasClass('parent')) {
                        //
                        // slide down/up child button group on direct event (not bubbling up) and set active-trail
                        $childButtonGroup = $clickedButton.find('>div.button-group');
                        if (ev.target == this) {
                            // direct event: toggle child button group and reset button selection
                            var topPos = $clickedButton.outerHeight(),
                                topGroupHeight = $button_container.height(),
                                groupHeight = $childButtonGroup.outerHeight(true);

                            if ($childButtonGroup.is(':hidden')) {
                                // slide down and set group visible
                                $childButtonGroup.css({"top": topPos + "px"}).slideDown(300);
                                $clickedButton.addClass('visible-children');

                                // set container height
                                $button_container.height(topGroupHeight + groupHeight);

                            } else {
                                $childButtonGroup.slideUp(300);
                                $clickedButton.removeClass('visible-children');
                                $button_container.height(topGroupHeight - groupHeight);
                            }

                            // deselect reset button, because filter of this button is active
                            // (usability: selection only on lowest level, no side-branches)
                            $button_container.find('.button.reset').removeClass('selected');

                        }
                        // set also on parent buttons (during event bubbling)
                        $clickedButton.addClass('active-trail');

                    } else {
                        //
                        // set filter for single or multiple selection inside button group
                        dataFilter = $clickedButton.attr('data-filter');

                        // toggle clicked button and its filter
                        if ($clickedButton.hasClass('selected')) {
                            // remove selection and filter
                            var index = groupFilter.indexOf(dataFilter);
                            if (index >= 0) groupFilter.splice(index,  1); // remove element at index
                            $clickedButton.removeClass('selected');

                        } else {
                            // add the filter to the filterGroup array
                            if (filterSettings.filter_multi_select) {
                                groupFilter.push(dataFilter);
                            } else {
                                $button_container.find('.button.selected').removeClass('selected');
                                groupFilter = [dataFilter];
                            }
                            // add selection
                            $clickedButton.addClass('selected');
                        }

                        // add/remove 'selected' from reset-button depending on other selected buttons
                        updateResetButton();

                    }

                    //
                    // create resulting filter for all filter container
                    groupFilterColl[buttonContainerId] = groupFilter;

                    // combine button container filters
                    // (cartesian product: [.color1, .color2] X [.type1, .type2] = ".color1.type1, .color1.type2, .color2.type1, .color2.type2")
                    var first = [], result = [];
                    for (var group in groupFilterColl) {
                        // set first array of cartesian product
                        if (result.length == 0) {
                            result = groupFilterColl[group];
                            continue;
                        } else {
                            first = result;
                            result = [];
                        }

                        // cartesian product of result array with current group filter array
                        var second = groupFilterColl[group];
                        for (var i = 0; i < first.length; i++) {
                            for (var j = 0; j < second.length; j++) {
                                result.push(first[i]+second[j]);
                            }
                        }
                    }

                    // filter the isotope container accordingly
                    var filterValue = result.join(", ");
                    $container.isotope({ filter: filterValue });
                });
            });

          }
        } catch(err) {
            // just make sure nothing went wrong
            console.log(err.message);
        }


        //
        // Attach sort button events: only available, if items are sortable and sort buttons exist on page
        //
        try {

            // check if sortable items and sort button groups have been defined
            var $sortButtons = Drupal.settings.isotope_sort;
            var hasSortButtons = (typeof $sortButtons != 'undefined') && Object.keys($sortButtons).length > 0;

            if (settings.sort_enabled && hasSortButtons) {

                // Iterate through  sort button containers (usually one) and add button click events
                $.each($sortButtons, function (buttonContainerId, sortSettings) {
                    //
                    // get button group with given id
                    var $sort_button_container = $('#' + buttonContainerId),
                        groupSortBy = [],
                        resetButtonSelection = function () {
                            $sort_button_container.find('.button.selected').removeClass('selected');
                        },
                        updateItemSortClasses = function (attributes) {
                            var $items = $container.find('div.isotope-item');
                            $items.removeClass('non-sortable').removeClass('sorted');
                            if (attributes != '') {
                                // items are sorted by at least one attribute
                                $items.addClass('sorted');
                                $container.find(attributes).addClass('non-sortable');
                            }
                        };

                    //
                    // attach button events
                    $sort_button_container.on('click', '.button', function() {
                        // disable uncover animation for all items
                        _disableUncoverAnimation();

                        // get sort value and update button selection
                        var dataSortBy,
                            $clickedButton = $(this);

                        if ($clickedButton.hasClass('reset')) {
                            // reset sort and clicked buttons
                            groupSortBy = [];
                            updateItemSortClasses('');
                            resetButtonSelection();
                            $clickedButton.addClass('selected');

                        } else {
                            // single or multiple selection inside button group
                            dataSortBy = $clickedButton.attr('data-sort-by');

                            // toggle clicked button and its filter
                            if ($clickedButton.hasClass('selected')) {
                                // remove selection and sort criteria
                                $clickedButton.removeClass('selected');
                                var index = groupSortBy.indexOf(dataSortBy);
                                groupSortBy.splice(index,  1); // remove element at index

                            } else {
                                // add selection and sort criteria
                                if (sortSettings.sort_multi_level) {
                                    groupSortBy.push(dataSortBy);
                                } else {
                                    groupSortBy = [dataSortBy];
                                    resetButtonSelection();
                                }
                                $clickedButton.addClass('selected');
                            }

                            // add/remove 'selected' from 'None' button depending on selected filter(s)
                            if (groupSortBy.length > 0) {
                                $sort_button_container.find('.button.reset').removeClass('selected');
                            } else {
                                $sort_button_container.find('.button.reset').addClass('selected');
                            }

                            // add .unsorted class to all not sortable items (attribute is undefined)
                            var attributes = '';
                            for (var i = 0; i < groupSortBy.length; i++) {
                                var attr = settings.sort_data[groupSortBy[i]];
                                attr = attr.replace(']', '*="{undef}"]')
                                attributes += (i == 0) ? attr : ', ' + attr;

                            }
                            updateItemSortClasses(attributes);
                        }

                        // sort the isotope container accordingly
                        $container.isotope({ sortBy: groupSortBy.concat(settings.sort_by) });
                    });

                });

            }

        } catch(err) {
            // just make sure nothing went wrong
            console.log(err.message);
        }


        //
        // Initialize Isotope container and item uncovering effect (animated uncovering of items during scrolling and resizing)
        //
        // Set isotope options
        var $options = new Object();
        $options.containerStyle = {
            position: "relative"
        };
        $options.itemSelector  = settings.item_selector;
        $options.isInitLayout  = true;       // set to false, if layout should be triggered manually
        $options.isResizeBound = settings.resizable;
        $options.isOriginLeft  = true;
        $options.isOriginTop   = true;
        $options.transitionDuration = settings.trans_duration+'ms';
        $options.visibleStyle  = {
            opacity: 1,
            transform: 'scale(1.0)'
        };
        $options.hiddenStyle   = {
            opacity: 0,
            transform: 'scale(0.001)'
        };

        // set layout mode specific options
        switch (settings.layout_mode) {
            case 'masonry':
                $options.layoutMode = 'masonry';
                $options.masonry = {
                    columnWidth: settings.grid_sizer,
                    gutter: settings.gutter_sizer
                };
                break;

            case 'masonryHorizontal':
                $options.layoutMode = 'masonryHorizontal';
                $options.masonryHorizontal = {
                    rowHeight: settings.grid_sizer,
                    gutter: settings.gutter_sizer
                };
                break;
            case 'fitRows':
                $options.layoutMode = 'fitRows';
                break;

            case 'fitColumns':
                $options.layoutMode = 'fitColumns';
                break;

            case 'cellsByRow':
                $options.layoutMode = 'cellsByRow';
                $options.cellsByRow = {
                    columnWidth: settings.grid_sizer,
                    rowHeight: settings.grid_sizer
                };
                break;
            case 'cellsByColumn':
                $options.layoutMode = 'cellsByColumn';
                $options.cellsByColumn = {
                    columnWidth: settings.grid_sizer,
                    rowHeight: settings.grid_sizer
                };
                break;

            case 'horizontal':
                $options.layoutMode = 'horizontal';
                $options.horizontal = {
                    verticalAlignment: settings.alignment
                };
                break;

            case 'vertical':
                $options.layoutMode = 'vertical';
                $options.vertical = {
                    horizontalAlignment: settings.alignment
                };
                break;
        }

        // add a stamp element to the container
        $options.stamp = settings.stamp_elements;

        // sorting
        $options.sortBy        = settings.sort_by;
        $options.sortAscending = true;
        if (settings.sort_enabled) {
            $options.getSortData = settings.sort_data;
        }


        //
        // Initialize item uncovering occurring during scrolling and resizing
        //
        var $items = $container.find('div.isotope-item'),
            uncoverEffectEnabled = settings.uncover_effect_enabled,
            uncoveredItems = 0,
            isScrolling = false,
            resizeTimeout = null,
            // 0 = item is considered in the viewport as soon as it enters,
            // 1 = item is considered in the viewport only when it's fully inside
            threshold = 0.2,
            _updateUncoveredStatus = function () {
                // checks if all items are uncovered and detach events, when true
                uncoveredItems++;
                if (uncoveredItems >= $items.length) {
                    $(window).off('scroll', _onScroll);
                    $(window).off('resize', _onResize);
                }
            },
            _inViewport = function (elem) {
                var elemH = elem.height(),
                    elemTop = elem.offset().top,
                    elemBottom = elemTop + elemH,
                    viewedH = $(window).height() + window.scrollY,
                    inViewport = ( (elemTop + elemH * threshold) <= viewedH ) && ( (elemBottom - elemH * threshold) >= window.scrollY );
                return inViewport;
            },
            _uncoverItems = function () {
                // check each item to be uncovered
                $items.each(function () {
                    var $this = $(this);
                    if (!$this.hasClass('shown') && !$this.hasClass('animate') && _inViewport($this)) {
                        var perspY = $(window).height() / 2 + window.scrollY;
                        $this.css({
                            perspectiveOrigin: '20% ' + perspY + 'px'
                            /*animationDuration: '0.6s'   */
                        });
                        $this.addClass('animate');
                        _updateUncoveredStatus();
                        $this.one('webkitAnimationEnd oanimationend msAnimationEnd animationend', function () {
                            // disable animation when finished
                            $this.addClass('shown').removeClass('animate');
                        });
                    }
                });
                isScrolling = false;
            },
            _disableUncoverAnimation = function () {
                // disables uncover animation and shows all items
                $items.each(function () {
                    $(this).addClass('shown').removeClass('animate');
                });
            },
            _onScroll = function () {
                // uncover items on scrolling
                if (!isScrolling) {
                    isScrolling = true;
                    setTimeout(function () { _uncoverItems(); }, 60);
                }
            },
            _onResize = function () {
                // uncover items when resizing window
                if (resizeTimeout) {
                    clearTimeout(resizeTimeout); // clear timeout as long as resizing events occur
                }
                resizeTimeout = setTimeout(function () {
                    _uncoverItems();
                    resizeTimeout = null; // resize done
                }, 1000);
            };


        //
        // Apply Isotope to container, when document and images are loaded
        //
        $container.imagesLoaded(function () {

          // apply isotope options
          $container.isotope($options);

          // initialize items
          if (uncoverEffectEnabled) {
              $items.each(function () {
                  var $this = $(this);
                  if (_inViewport($this)) {
                      $this.addClass('shown');
                      _updateUncoveredStatus();
                  }
              });

              // attach scroll and resize events
              $(window).on('scroll', _onScroll);
              $(window).on('resize', _onResize);

          } else {
              // make all item visible at once
              _disableUncoverAnimation();
          }

        });

      });  // each isotope settings

    }
  };

})(jQuery);

