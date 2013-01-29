/* -*- mode: c++; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: t -*-
 * vim: ts=4 sw=4 noet ai cindent syntax=cpp
 *
 * Conky, a system monitor, based on torsmo
 *
 * Please see COPYING for details
 *
 * Copyright (C) 2012 Pavel Labath et al.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TABLE_LAYOUT_HH
#define TABLE_LAYOUT_HH

#include <map>
#include <vector>

#include "list_map.hh"
#include "layout-item.hh"
#include "luamm.hh"

namespace conky {

	class table_layout: public layout_item {
	public:
		enum class alignment { LEFT, CENTER, RIGHT };

	private:
		struct column {
			uint32_t width;
			alignment align;
		};
		struct item_data {
			point pos;
			point size;
		};

		typedef std::vector<std::shared_ptr<layout_item>> ItemRow;
		typedef std::vector<item_data> DataRow;
		typedef std::vector<DataRow> DataGrid;

		struct data {
			std::unique_ptr<const output_method::scope> scope;
			DataGrid grid;

			data() { }

			data(std::unique_ptr<const output_method::scope> &&scope_, size_t cols, size_t rows)
				: scope(std::move(scope_)), grid(cols, DataRow(rows))
			{}
		};

		typedef list_map<const output_method *, data> DataMap;

		lua::state &l;
		std::vector<column> columns;
		std::vector<ItemRow> item_grid;
		DataMap data_map;
		std::mutex data_mutex;
		int scope_ref;

		static column default_column;

		size_t read_columns();
		static column read_column(lua::state &l, size_t colno);
		static point::type align(point::type have, point::type need, alignment a);
		ItemRow read_row(lua::state &l, size_t rowno, size_t cols);
		std::shared_ptr<layout_item> read_cell(lua::state &l, size_t rowno, size_t colno);
		DataMap::iterator make_data(output_method &om);

		bool empty() const { return item_grid.empty() || item_grid[0].empty(); }

	public:
		table_layout(lua::state &l_);
		virtual ~table_layout() { l.unref(lua::REGISTRYINDEX, scope_ref); }

		virtual point size(output_method &om);

		virtual void draw(output_method &om, const point &p, const point &size);
	};
}

#endif /* TABLE_LAYOUT_HH */