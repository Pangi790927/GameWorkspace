#ifndef VERTEX_H
#define VERTEX_H

#include "Util.h"

template <typename Type, typename Desc, typename ...Args>
struct VertexNode : VertexNode<Args...> {
	using DataType = Type;
	using DescType = Desc;
	using ChildNode = VertexNode<Args...>;
	
	DataType data;

	constexpr static int count() {
		return 1 + ChildNode::count();
	}

	template <typename QueryDesc>
	constexpr static bool has_desc() {
		return Util::same_class<QueryDesc, DescType>::value ||
				ChildNode:: template has_desc<QueryDesc>();
	}

	template <typename QueryDesc>
	struct get_type {
		using type = typename Util::if_true<
			DataType,
			typename ChildNode::template get_type<QueryDesc>::type,
			Util::same_class<QueryDesc, DescType>::value
		>::type;
	};

	template <int index>
	struct get_desc {
		using type = typename Util::if_true<
			DescType,
			typename ChildNode::template get_desc<index - 1>::type,
			index == 0
		>::type;
	};

	template <typename QueryDesc>
	typename std::enable_if<
			!Util::same_class<QueryDesc, DescType>::value,
			typename get_type<QueryDesc>::type&>::type
	get() {
		return ChildNode::template get<QueryDesc>();
	}

	template <typename QueryDesc>
	typename std::enable_if<
			Util::same_class<QueryDesc, DescType>::value,
			typename get_type<QueryDesc>::type&>::type
	get() {
		return data;
	}

	template <typename FuncType>
	void mapFunc (FuncType&& func) {
		func(*this);
		ChildNode::mapFunc(func);
	}

	template <typename FuncType>
	void mapFunc (FuncType&& func) const {
		func(*this);
		ChildNode::mapFunc(func);
	}
};

template <typename Type, typename Desc>
struct VertexNode<Type, Desc> {
	using DataType = Type;
	using DescType = Desc;

	DataType data;

	constexpr static int count() {
		return 1;
	}

	template <typename QueryDesc>
	constexpr static bool has_desc() {
		return Util::same_class<QueryDesc, DescType>::value || false;
	}

	template <typename QueryDesc>
	struct get_type {
		using type = typename Util::if_true<
			DataType, 
			void,
			Util::same_class<QueryDesc, DescType>::value
		>::type;
	};

	template <int index>
	struct get_desc {
		using type = typename Util::if_true<
			DescType,
			void,
			index == 0
		>::type;
	};

	template <typename QueryDesc>
	typename get_type<QueryDesc>::type& get() {
		return data;
	}

	template <typename FuncType>
	void mapFunc (FuncType&& func) {
		func(*this);
	}

	template <typename FuncType>
	void mapFunc (FuncType&& func) const {
		func(*this);
	}
};

template <typename DataType, typename Descriptor, typename ...Args>
struct Vertex : VertexNode<DataType, Descriptor, Args...> {
	using Node = VertexNode<DataType, Descriptor, Args...>;

	constexpr const static int nodeCount = Node::count();
	
	template <typename DescType>
	struct get_type {
		using type = typename Node::template get_type<DescType>::type;
	};

	template <int index>
	struct get_desc {
		using type = typename Node::template get_desc<index>::type;
	};

	template <int index>
	struct get_type_i {
		using type = typename get_type<typename get_desc<index>::type>::type;
	};

	template <int index>
	typename get_type_i<index>::type&
	getAt() {
		return Node::template get<typename get_desc<index>::type>();
	}

	Vertex() {}

	template <typename ...FillArgs>
	Vertex (FillArgs ...args) {
		fill<nodeCount>(args...);
	}

	template <int index, typename FillArg, typename ...FillArgs>
	void fill (const FillArg& arg, FillArgs ...args) {
		getAt<nodeCount - index>() = arg;
		fill<index - 1> (args...);
	}

	template <int index, typename FillArg>
	void fill (const FillArg& arg) {
		getAt<nodeCount - index>() = arg;
	}

	template <typename QueryDesc, typename FuncType>
	typename std::enable_if<Node::template has_desc<QueryDesc>(), void>::type
	useIfExists (FuncType&& func) {
		func(Node::template get<QueryDesc>());
	}

	template <typename QueryDesc, typename FuncType>
	typename std::enable_if<!Node::template has_desc<QueryDesc>(), void>::type
	useIfExists (FuncType&& func) {
		;
	}

	template <typename QueryDesc, typename ArgType>
	typename std::enable_if<Node::template has_desc<QueryDesc>(), void>::type
	setIfExists (ArgType&& val) {
		Node::template get<QueryDesc>() = val;
	}

	template <typename QueryDesc, typename ArgType>
	typename std::enable_if<!Node::template has_desc<QueryDesc>(), void>::type
	setIfExists (ArgType&& func) {
		;
	}

	friend std::ostream& operator << (std::ostream& stream, const Vertex& arg) {
		stream << "Node: " << std::endl;
		auto print = [&] (auto& node) {
			stream << std::remove_reference<decltype(node)>::type::DescType::name <<
				"\n" << node.data << std::endl;
		};
		arg.mapFunc(print);
		return stream;
	}
};

#endif