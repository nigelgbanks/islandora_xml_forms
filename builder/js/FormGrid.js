Ext.define('Ext.Form.Grid', {
  extend: 'Ext.grid.Panel',
  alias: 'widget.formgrid',
  collapsible: true,
  iconCls: 'icon-grid',
  selType: 'rowmodel',
  height: 200,
  plugins: {
    ptype: 'rowediting',
    clicksToEdit: 2
  },
  viewConfig: {
    plugins: {
      ptype: 'gridviewdragdrop',
      dragText: 'Drag and drop to reorganize'
    }
  },
  tbar: [{
    xtype: 'toolbar',
    items: [{
      iconCls: 'icon-add',
      text: 'Add',
      handler: function() {
        var grid = this.up('formgrid');
        console.log(grid);
        var rec = new grid.store.model;
        grid.store.insert(0, rec);
        var plugin = grid.getPlugin();
        plugin.cancelEdit();
        plugin.startEdit(rec, grid.columns[0]);
      }
    }, {
      iconCls: 'icon-delete',
      text: 'Delete',
      disabled: true,
      itemId: 'delete',
      handler: function() {
        var grid = this.up('formgrid');
        var selection = grid.getView().getSelectionModel().getSelection()[0];
        if (selection) {
          grid.store.remove(selection);
        }
      }
    }]
  }],
  listeners: {
    selectionchange: function(selModel, selections) {
      this.down('#delete').setDisabled(selections.length === 0);
    }
  }
});