#include "../include/json/NodeView.h"

using namespace json;

template struct NodeView<null_t>;
template struct NodeView<boolean_t>;
template struct NodeView<integral_t>;
template struct NodeView<real_t>;
template struct NodeView<string_t>;
template struct NodeView<array_t>;
template struct NodeView<object_t>;

template<json_type T>
void json::NodeView<T>::validate_node_type(NodeType const& node_type)
{
	if (node_type != NodeType::from_type<T>()) throw make_exception("NodeView<", typeid(T).name(), "> doesn't allow Nodes of type ", node_type);
}
