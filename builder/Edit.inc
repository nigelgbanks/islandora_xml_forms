<?php

/**
 * @file
 *
 * Callbacks and functions used in the Edit Form Page, for rendering the Form Builder GUI.
 */

/**
 * Includes all the required CSS/JS files needed to render the Form Builder GUI.
 *
 * @param string $form_name
 *   The name of the form to edit.
 *
 * @return string
 *   The html where the Form Builder GUI will be rendered.
 */
function xml_form_builder_edit($form_name) {
  module_load_include('inc', 'extjs', 'extjs.utils');
  extjs_add_drupal_reset_css_files();
  drupal_add_css(drupal_get_path('module', 'xml_form_builder') . '/css/ext-all-scoped-custom.css');
  drupal_add_css(drupal_get_path('module', 'xml_form_builder') . '/css/icons.css');
  drupal_add_js(drupal_get_path('module', 'xml_form_builder') . '/js/test.js');
  extjs_add_js_file('ext-all');
  drupal_add_js(drupal_get_path('module', 'xml_form_builder') . '/js/app.js');
  //  drupal_add_js(drupal_get_path('module', 'xml_form_builder') . '/js/FormGrid.js');
  /*extjs_add_all();
  xml_form_builder_edit_include_css();
  xml_form_builder_edit_include_js();*/
  xml_form_builder_create_element_type_store();
  xml_form_builder_create_element_store($form_name);
  xml_form_builder_create_properties_store($form_name);
  return '<div id="form-builder-container"></div>';
  //return '<div id="xml-form-builder-editor"></div>';
}

/**
 * Includes all the required CSS files needed to render the Form Builder GUI.
 */
function xml_form_builder_edit_include_css() {
  drupal_add_css(drupal_get_path('module', 'xml_form_builder') . '/css/EditForm.css');
}

/**
 * Includes all the required JS files needed to render the Form Builder GUI.
 */
function xml_form_builder_edit_include_js() {
  $js_path = drupal_get_path('module', 'xml_form_builder') . '/js';
  $files = array(
    'Formbuilder.js',
    'FormGrid.js',
    'Tooltips.js',
    'ElementModel.js',
    'PropertiesModel.js',
    'MainPanel.js',
    'DisplayPanel.js',
    'TreePanel.js',
    'ElementForm.js',
    'PropertiesForm.js',
    'PreviewPanel.js',
  );
  foreach ($files as $file) {
    drupal_add_js("$js_path/$file");
  }
}

/**
 * Save changes made to the form definition client side.
 *
 * Transforms the submited JSON into a Form Definition which is then stored in the database as an XML Form Definition.
 *
 * @param string $form_name
 *   The name of the form to update.
 */
function xml_form_builder_edit_save($form_name) {
  module_load_include('inc', 'xml_form_builder', 'JSONFormDefinition');
  module_load_include('inc', 'xml_form_builder', 'XMLFormDatabase');
  module_load_include('inc', 'xml_form_api', 'XMLFormDefinition');
  try {
    $definition = new JSONFormDefinition($_POST['data']);
    list($properties, $form) = $definition->getPropertiesAndForm();
    $definition = XMLFormDefinitionGenerator::Create($properties, $form);
    XMLFormDatabase::Update($form_name, $definition);
  } catch (Exception $e) {
    $msg = "File: {$e->getFile()}<br/>Line: {$e->getLine()}<br/>Error: {$e->getMessage()}";
    drupal_set_message(filter_xss($msg), 'error');
  }
}

/**
 * Creates an inline javascript file that creates an ExtJS store containing all the possible form field types.
 *
 * Used in the Element Type Combo Box, in the Element form of the Form Builder GUI.
 */
function xml_form_builder_create_element_type_store() {
  $types = xml_form_builder_get_element_types();
  sort($types);
  $data = array();
  foreach ($types as $type) {
    $data[] = array('display' => $type, 'value' => $type);
  }
  drupal_add_js(array('formbuilder' => array('element_types' => $data)), 'setting');
}

/**
 * Creates an inline javascript file that creates an ExtJS store containing the form properties.
 *
 * Used to populate the 'Form Properties' Form.
 *
 * @param string $form_name
 *   The name of the form which properties will be used.
 */
function xml_form_builder_create_properties_store($form_name) {
  module_load_include('inc', 'xml_form_builder', 'XMLFormDatabase');
  module_load_include('inc', 'xml_form_builder', 'JSONFormDefinitionGenerator');
  module_load_include('inc', 'xml_form_api', 'XMLFormDefinition');
  $definition = new XMLFormDefinition(XMLFormDatabase::Get($form_name));
  $data = JSONFormDefinitionGenerator::GetProperties($definition->getProperties());
  drupal_add_js(array('formbuilder' => array('properties' => $data)), 'setting');
}

/**
 * Creates an inline javascript file that creates an ExtJS store containing the form elements.
 *
 * Used to populate the 'Elements Tree'.
 *
 * @param string $form_name
 *   The name of the form which will be used.
 */
function xml_form_builder_create_element_store($form_name) {
  module_load_include('inc', 'xml_form_builder', 'XMLFormDatabase');
  module_load_include('inc', 'xml_form_builder', 'JSONFormDefinitionGenerator');
  module_load_include('inc', 'xml_form_api', 'XMLFormDefinition');
  $definition = new XMLFormDefinition(XMLFormDatabase::Get($form_name));
  $data = JSONFormDefinitionGenerator::GetForm($definition->getForm());
  drupal_add_js(array('formbuilder' => array('elements' => $data)), 'setting');
}

/**
 * Gets a list of all the existing element types defined by the elements hook.
 *
 * @staticvar array $cache
 *   The cached list of existing element types.
 *
 * @return array
 *   A list of all the existing element types.
 */
function xml_form_builder_get_element_types() {
  static $cache;
  if (!isset($cache)) {
    $cache = array();
    //XXX: --Adam Errr...  why not just call module_invoke_all?
    foreach (module_implements('elements') as $module) {
      $elements = module_invoke($module, 'elements');
      if (isset($elements) && is_array($elements)) {
        $cache = array_merge_recursive($cache, $elements);
      }
    }
  }
  $types = array_keys($cache);
  sort($types);
  $output = array();
  foreach ($types as $type) {
    $output[$type] = $type;
  }
  return $output;
}