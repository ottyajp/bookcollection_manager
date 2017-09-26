require 'inifile'
require 'amazon/ecs'

inifile = IniFile.load('./paapi.conf')

Amazon::Ecs.options = {
  associate_tag: inifile['keys']['tag'],
  AWS_access_key_id: inifile['keys']['aki'],
  AWS_secret_key: inifile['keys']['sek'],
  country: :jp
}

res = Amazon::Ecs.item_lookup(ARGV[0], response_group: 'Medium,Images')

p res.items.map {|item| item.get('ItemAttributes/Title') }
p res.items.map {|item| item.get('ItemAttributes/Author') }
p res.items.map {|item| item.get('MediumImage/URL') }
