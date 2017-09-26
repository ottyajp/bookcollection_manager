require 'amazon/ecs'

Amazon::Ecs.options = {
  associate_tag: 'ottyajp-22',
  AWS_access_key_id: 'AKIAJ5F7RBN4POBHT5ZA',
  AWS_secret_key: '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',
  country: :jp
}

res = Amazon::Ecs.item_lookup(ARGV[0], response_group: 'Small')

p res.items.map {|item| item.get('ItemAttributes/Title') }
p res.items.map {|item| item.get('ItemAttributes/Author') }
